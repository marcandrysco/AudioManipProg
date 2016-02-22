#include "common.h"


/**
 * Engine structure.
 *   @core: The core.
 *   @instr: The instrument.
 *   @effect: The effect.
 *   @comm: MIDI device communcation.
 */

struct amp_engine_t {
	struct amp_core_t *core;
	struct amp_instr_t instr;
	struct amp_effect_t effect[2];

	struct amp_comm_t *comm;
};


/*
 * local declarations
 */

static void callback(double **buf, unsigned int len, void *arg);


/**
 * Create an engine.
 *   @comm: Consumed. Optional. The communication structure.
 *   &returns: The engine.
 */

struct amp_engine_t *amp_engine_new(struct amp_comm_t *comm)
{
	struct amp_engine_t *engine;

	engine = malloc(sizeof(struct amp_engine_t));
	engine->core = amp_core_new(96000);
	engine->instr = amp_instr_null;
	engine->effect[0] = amp_effect_null;
	engine->effect[1] = amp_effect_null;
	engine->comm = comm ?: amp_comm_new();

	return engine;
}

/**
 * Delete an engine.
 *   @engine: The engine.
 */

void amp_engine_delete(struct amp_engine_t *engine)
{
	amp_comm_delete(engine->comm);
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
	struct amp_engine_t *engine;

	engine = amp_engine_new(comm);
	amp_engine_update(engine, "test.ml");

	amp_audio_exec(audio, callback, engine);

	fgetc(stdin);

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
	struct amp_event_t event;
	struct amp_engine_t *engine = arg;

	if(len == 0) {
		printf("xrun\n");
		return;
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
		amp_instr_proc(engine->instr, buf, NULL, len);
	else {
		unsigned int i;

		for(i = 0; i < len; i++)
			buf[0][i] = buf[1][i] = buf[0][i] + buf[1][i];
	}

	if(engine->effect[0].iface != NULL)
		amp_effect_proc(engine->effect[0], buf[0], NULL, len);

	if(engine->effect[1].iface != NULL)
		amp_effect_proc(engine->effect[1], buf[1], NULL, len);
}
