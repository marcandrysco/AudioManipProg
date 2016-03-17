#include "common.h"


/*
 * local declarations
 */

static void callback(double **buf, unsigned int len, void *arg);
static void notify(const char *path, void *arg);


/**
 * Create an engine.
 *   @list: Constant. Optional. The file list.
 *   @comm: Consumed. Optional. The communication structure.
 *   &returns: The engine.
 */

struct amp_engine_t *amp_engine_new(char **list, struct amp_comm_t *comm)
{
	struct amp_engine_t *engine;

	engine = malloc(sizeof(struct amp_engine_t));
	engine->rev = 1;
	engine->lock = sys_mutex_init(0);
	engine->sync = sys_mutex_init(0);
	engine->run = engine->toggle = false;
	engine->core = amp_core_new(96000);
	engine->clock = amp_basic_clock(amp_basic_new(120.0, 4.0, 96000));
	engine->seq = amp_seq_null;
	engine->instr = amp_instr_null;
	engine->effect[0] = amp_effect_null;
	engine->effect[1] = amp_effect_null;
	engine->comm = comm ?: amp_comm_new();
	engine->notify = amp_notify_new(list, notify, engine);

	return engine;
}

/**
 * Delete an engine.
 *   @engine: The engine.
 */

void amp_engine_delete(struct amp_engine_t *engine)
{
	amp_notify_delete(engine->notify);
	amp_comm_delete(engine->comm);
	amp_clock_delete(engine->clock);
	amp_seq_erase(engine->seq);
	amp_instr_erase(engine->instr);
	amp_effect_erase(engine->effect[0]);
	amp_effect_erase(engine->effect[1]);
	amp_core_delete(engine->core);
	sys_mutex_destroy(&engine->lock);
	sys_mutex_destroy(&engine->sync);
	free(engine);
}


/**
 * Update the engine with the code from the given path.
 *   @engine: The engine.
 *   @path: The path.
 */

void amp_engine_update(struct amp_engine_t *engine, const char *path)
{
	char *err;
	struct ml_value_t *value;
	struct amp_box_t *box;
	struct ml_env_t *env;

	sys_mutex_lock(&engine->sync);
	sys_mutex_lock(&engine->lock);

	env = amp_core_eval(engine->core, path, &err);
	if(env == NULL) {
		fprintf(stderr, "%s\n", err), free(err); return;
	}

	value = ml_env_lookup(env, "amp.clock");
	if(value != NULL) {
		box = amp_unbox_value(value, amp_box_clock_e);
		if(box != NULL)
			amp_clock_set(&engine->clock, amp_clock_copy(box->data.clock));
		else
			fprintf(stderr, "Type for 'seq' is not valid.\n");
	}

	value = ml_env_lookup(env, "amp.seq");
	if(value != NULL) {
		box = amp_unbox_value(value, amp_box_seq_e);
		if(box != NULL)
			amp_seq_set(&engine->seq, amp_seq_copy(box->data.seq));
		else
			fprintf(stderr, "Type for 'seq' is not valid.\n");
	}

	value = ml_env_lookup(env, "amp.instr");
	if(value != NULL) {
		box = amp_unbox_value(value, amp_box_instr_e);
		if(box != NULL)
			amp_instr_set(&engine->instr, amp_instr_copy(box->data.instr));
		else
			fprintf(stderr, "Type for 'instr' is not valid.\n");
	}

	value = ml_env_lookup(env, "amp.effect");
	if(value != NULL) {
		box = amp_unbox_value(value, amp_box_effect_e);
		if(box != NULL) {
			amp_effect_set(&engine->effect[0], amp_effect_copy(box->data.effect));
			amp_effect_set(&engine->effect[1], amp_effect_copy(box->data.effect));
		}
		else
			fprintf(stderr, "Type for 'effect' is not valid.\n");
	}

	value = ml_env_lookup(env, "amp.run");
	if(value != NULL) {
		if(value->type != ml_value_bool_e)
			fprintf(stderr, "Type mismatch. Variable 'amp.run' must be a 'bool'.\n");

		engine->toggle = value->data.flag;
	}
#if DEBUG
	struct ml_env_t *iter;

	for(iter = env; iter != NULL; iter = iter->up) {
		if(iter->id[0] != '_')
			continue;

		printf("%s: ", iter->id);
		ml_value_print(iter->value, stdout);
		printf("\n");
	}
#endif

	engine->rev++;
	sys_mutex_unlock(&engine->lock);
	sys_mutex_unlock(&engine->sync);

	ml_env_delete(env);
}


/**
 * Execute the audio engine.
 *   @audio: The audio device.
 *   @file: The file list.
 *   @plugin: The plugin list.
 *   @comm: Optional. Consumed. The communication structure.
 */

void amp_exec(struct amp_audio_t audio, char **file, char **plugin, struct amp_comm_t *comm)
{
	char **ml, **el;
	struct amp_engine_t *engine;

	engine = amp_engine_new(file, comm);

	for(el = plugin; *el != NULL; el++) {
		char *err;

		err = amp_core_plugin(engine->core, *el);
		if(err != NULL)
			fprintf(stderr, "Warning. %s\n", err);
	}

	for(ml = file; *ml != NULL; ml++)
		amp_engine_update(engine, *ml);

	amp_audio_exec(audio, callback, engine);

	/*
	while(true) {
		unsigned int n = amp_http_poll(http, NULL) + 1;
		struct sys_poll_t poll[n];

		poll[0] = sys_poll_fd(STDIN_FILENO, sys_poll_in_e);
		amp_http_poll(http, poll+1);

		sys_poll(poll, n, -1);

		if(poll[0].revents)
			break;

		amp_http_proc(http, poll+1);
	}

	int c;
	while((c = getchar()) != '\n' && c != EOF);
	*/

	bool quit = false;
	while(!quit) {
		unsigned int argc;
		char **argv, buf[256];

		printf("> ");
		if(fgets(buf, sizeof(buf), stdin) == NULL)
			break;

		argv_parse(buf, &argv, &argc);

		if(argc > 0) {
			if((strcmp(argv[0], "q") == 0) || (strcmp(argv[0], "quit") == 0) || (strcmp(argv[0], "exit") == 0))
				quit = true;
			else if(strcmp(argv[0], "s") == 0) {
				printf("%s engine.\n", engine->toggle ? "Stopping" : "Starting");
				engine->toggle ^= true;
			}
			else
				printf("Unknown command '%s'.\n", argv[0]);
		}

		argv_free(argv);
	}

	amp_audio_halt(audio);
	amp_engine_delete(engine);
}


/**
 * Audio callback.
 *   @buf: The buffer.
 *   @len: The length.
 *   @arg: The argument.
 */

static void callback(double **buf, unsigned int len, void *arg)
{
	bool run;
	struct amp_event_t event;
	struct amp_engine_t *engine = arg;
	struct amp_time_t time[len+1];

	if(len == 0) {
		//printf("xrun\n");
		return;
	}

	if(!sys_mutex_trylock(&engine->lock))
		return;

	run = engine->toggle;
	if(engine->run != run) {
		struct amp_seek_t seek;

		engine->run = run;
		amp_clock_info(engine->clock, (run ? amp_info_start : amp_info_stop)(&seek));
	}

	amp_clock_proc(engine->clock, time, len);

	if(engine->seq.iface != NULL) {
		unsigned int i;
		struct amp_queue_t queue;

		amp_queue_init(&queue);
		amp_seq_proc(engine->seq, &queue, time, len);

		for(i = 0; i < queue.idx; i++) {
			if(engine->instr.iface != NULL)
				amp_instr_info(engine->instr, amp_info_action(&queue.arr[i]));

			if(engine->effect[0].iface != NULL)
				amp_effect_info(engine->effect[0], amp_info_action(&queue.arr[i]));

			if(engine->effect[1].iface != NULL)
				amp_effect_info(engine->effect[1], amp_info_action(&queue.arr[i]));
		}
	}

	while(amp_comm_read(engine->comm, &event)) {
		struct amp_action_t action = { 0, event };

		if(engine->instr.iface != NULL)
			amp_instr_info(engine->instr, amp_info_action(&action));

		if(engine->effect[0].iface != NULL)
			amp_effect_info(engine->effect[0], amp_info_action(&action));

		if(engine->effect[1].iface != NULL)
			amp_effect_info(engine->effect[1], amp_info_action(&action));
	}

	if(engine->instr.iface != NULL)
		amp_instr_proc(engine->instr, buf, time, len);
	else {
		unsigned int i;

		for(i = 0; i < len; i++)
			buf[0][i] = buf[1][i] = buf[0][i] + buf[1][i];
	}

	if(engine->effect[0].iface != NULL)
		amp_effect_proc(engine->effect[0], buf[0], time, len);

	if(engine->effect[1].iface != NULL)
		amp_effect_proc(engine->effect[1], buf[1], time, len);

	sys_mutex_unlock(&engine->lock);
}

/**
 * Handle a change notification.
 *   @path: The path.
 *   @arg: The argument.
 */

static void notify(const char *path, void *arg)
{
	amp_engine_update(arg, path);
}
