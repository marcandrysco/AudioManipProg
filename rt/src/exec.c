#include "common.h"


/**
 * Engine structure.
 *   @run, toggle: The run and toggle flag.
 *   @core: The core.
 *   @notify: The notifier.
 *   @clock: The clock.
 *   @seq: The sequencer.
 *   @instr: The instrument.
 *   @effect: The effect.
 *   @comm: MIDI device communcation.
 */

struct amp_engine_t {
	bool run, toggle;
	struct amp_core_t *core;
	struct amp_notify_t *notify;

	struct amp_clock_t clock;
	struct amp_seq_t seq;
	struct amp_instr_t instr;
	struct amp_effect_t effect[2];

	struct amp_comm_t *comm;
};


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
	engine->run = engine->toggle = false;
	engine->core = amp_core_new(96000);
	engine->clock = amp_basic_clock(amp_basic_new(120.0, 4.0, 96000));
	engine->seq = amp_seq_null;
	engine->instr = amp_instr_null;
	engine->effect[0] = amp_effect_null;
	engine->effect[1] = amp_effect_null;
	engine->comm = comm ?: amp_comm_new();
	engine->notify = amp_notify_new(list, notify, engine);

	engine->toggle = true;

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

	env = amp_core_eval(engine->core, path, &err);
	if(env == NULL) {
		fprintf(stderr, "%s\n", err), free(err); return;
	}

	value = ml_env_lookup(env, "seq");
	if(value != NULL) {
		box = amp_unbox_value(value, amp_box_seq_e);
		if(box != NULL)
			amp_seq_set(&engine->seq, amp_seq_copy(box->data.seq));
		else
			fprintf(stderr, "Type for 'effect' is not valid.\n");
	}

	value = ml_env_lookup(env, "instr");
	if(value != NULL) {
		box = amp_unbox_value(value, amp_box_instr_e);
		if(box != NULL)
			amp_instr_set(&engine->instr, amp_instr_copy(box->data.instr));
		else
			fprintf(stderr, "Type for 'effect' is not valid.\n");
	}

	value = ml_env_lookup(env, "effect");
	if(value != NULL) {
		box = amp_unbox_value(value, amp_box_effect_e);
		if(box != NULL) {
			amp_effect_set(&engine->effect[0], amp_effect_copy(box->data.effect));
			amp_effect_set(&engine->effect[1], amp_effect_copy(box->data.effect));
		}
		else
			fprintf(stderr, "Type for 'effect' is not valid.\n");
	}

#if DEBUG
	struct ml_bind_t *bind;

	for(bind = env->bind; bind != NULL; bind = bind->next) {
		if(bind->id[0] != '_')
			continue;

		printf("%s: ", bind->id);
		ml_value_print(bind->value, stdout);
		printf("\n");
	}
#endif

	ml_env_delete(env);
}


/**
 * Execute the audio engine.
 *   @audio: The audio device.
 *   @comm: Optional. Consumed. The communication structure.
 */

void amp_exec(struct amp_audio_t audio, struct amp_comm_t *comm)
{
	bool quit = false;
	struct amp_engine_t *engine;

	engine = amp_engine_new((char *[]){ "test.ml", NULL }, comm);
	amp_engine_update(engine, "test.ml");

	amp_audio_exec(audio, callback, engine);

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
