#include "common.h"


/*
 * local declarations
 */
static void callback(double **buf, unsigned int len, void *arg);


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

	sys_mutex_lock(&engine->sync);
	sys_mutex_lock(&engine->lock);

	if(engine->run)
		amp_engine_stop(engine);

	double bar;
	amp_clock_info(engine->clock, amp_info_tell(&bar));

	value = ml_env_lookup(env, "amp.clock");
	if(value != NULL) {
		box = amp_unbox_value(value, amp_box_clock_e);
		if(box != NULL) {
			amp_clock_set(&engine->clock, amp_clock_copy(box->data.clock));
			amp_clock_info(engine->clock, amp_info_init());
		}
		else
			fprintf(stderr, "Type for 'amp.clock' is not valid.\n");
	}

	value = ml_env_lookup(env, "amp.instr");
	if(value != NULL) {
		box = amp_unbox_value(value, amp_box_instr_e);
		if(box != NULL) {
			amp_instr_set(&engine->instr, amp_instr_copy(box->data.instr));
			amp_instr_info(engine->instr, amp_info_init());
		}
		else
			fprintf(stderr, "Type for 'amp.instr' is not valid.\n");
	}

	value = ml_env_lookup(env, "amp.bar");
	if(value != NULL) {
		if(ml_value_isnum(value)) {
		}
		else
			fprintf(stderr, "Type mismatch. Variable 'amp.run' must be a 'bool'.\n");
	}

	value = ml_env_lookup(env, "amp.run");
	if(value != NULL) {
		if(value->type == ml_value_bool_v) {
			if(value->data.flag)
				amp_engine_start(engine);
		}
		else
			fprintf(stderr, "Type mismatch. Variable 'amp.run' must be a 'bool'.\n");
	}

#if DEBUG
	struct ml_env_t *iter;

	for(iter = env; iter != NULL; iter = iter->up) {
		if(iter->id[0] != '_')
			continue;

		printf("%s: %C\n", iter->id, ml_value_chunk(iter->value));
	}
#endif

	engine->rev++;

	struct amp_watch_t *watch;

	for(watch = engine->watch; watch != NULL; watch = watch->next)
		watch->func(env, watch->arg);

	//amp_clock_info(engine->clock, amp_info_seek(&bar));
	//amp_instr_info(engine->instr, amp_info_seek(&bar));

	sys_mutex_unlock(&engine->lock);
	sys_mutex_unlock(&engine->sync);

	ml_env_delete(env);
}


/**
 * Execute the audio engine.
 *   @audio: The audio device.
 *   @file: The file.
 *   @plugin: The plugin list.
 *   @comm: Optional. Consumed. The communication structure.
 */
void amp_exec(struct amp_audio_t audio, const char *file, char **plugin, struct amp_comm_t *comm)
{
	bool quit = false;
	char **el;
	struct amp_engine_t *engine;

	engine = amp_engine_new(file, comm, audio);

	for(el = plugin; *el != NULL; el++) {
		char *err;

		err = amp_core_plugin(engine->core, *el);
		if(err != NULL)
			fprintf(stderr, "Warning. %s\n", err), free(err);
	}

	amp_engine_update(engine, file);
	amp_audio_exec(audio, callback, engine);

	while(!quit) {
		unsigned int argc;
		char **argv, buf[256];

		printf("> ");
		fflush(stdout);
		if(fgets(buf, sizeof(buf), stdin) == NULL)
			break;

		argv_parse(buf, &argv, &argc);

		if(argc > 0) {
			if((strcmp(argv[0], "q") == 0) || (strcmp(argv[0], "quit") == 0) || (strcmp(argv[0], "exit") == 0))
				quit = true;
			else if(strcmp(argv[0], "s") == 0) {
				printf("%s engine.\n", engine->run ? "Stopping" : "Starting");
				(engine->run ? amp_engine_stop : amp_engine_start)(engine);
			}
			else
				printf("Unknown command '%s'.\n", argv[0]);
		}

		argv_free(argv);
	}

	amp_audio_halt(audio);
	amp_engine_delete(engine);
}

#include <sndfile.h>
SNDFILE *file = NULL;

/**
 * Audio callback.
 *   @buf: The buffer.
 *   @len: The length.
 *   @arg: The argument.
 */
static void callback(double **buf, unsigned int len, void *arg)
{
	struct amp_event_t event;
	struct amp_engine_t *engine = arg;
	struct amp_time_t time[len+1];

	if(len == 0) {
		printf("xrun\n");
		return;
	}

	if(!sys_mutex_trylock(&engine->lock)) {
		dsp_zero_d(buf[0], len);
		dsp_zero_d(buf[1], len);

		return;
	}

	/*
	run = engine->toggle;
	if(engine->run != run) {
		struct amp_seek_t seek;

		engine->run = run;
		amp_clock_info(engine->clock, (run ? amp_info_start : amp_info_stop)(&seek));
	}
	*/

	amp_clock_proc(engine->clock, time, len);

	struct amp_queue_t queue;

	amp_queue_init(&queue);

	while(amp_comm_read(engine->comm, &event))
		amp_queue_add(&queue, (struct amp_action_t){ 0, event });

	if(engine->instr.iface != NULL)
		amp_instr_proc(engine->instr, buf, time, len, &queue);
	else
		dsp_zero_d(buf[0], len), dsp_zero_d(buf[1], len);

	static int a = 0;
	if(a < 96000ull * 45824ull / 44100ull) {
		sf_write_double(file, buf[0], len);
		a+=len;
	}

	sys_mutex_unlock(&engine->lock);
}
