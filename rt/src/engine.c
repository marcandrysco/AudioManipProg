#include "common.h"

/*
 * local declarations
 */
static void notify(const char *path, void *arg);

#include <sndfile.h>
extern SNDFILE *file;
/**
 * Create an engine.
 *   @list: Constant. Optional. The file list.
 *   @comm: Consumed. Optional. The communication structure.
 *   @audio: The audio interface.
 *   &returns: The engine.
 */
struct amp_engine_t *amp_engine_new(const char *list, struct amp_comm_t *comm, struct amp_audio_t audio)
{
	const char *iface;
	struct amp_engine_t *engine;

	engine = malloc(sizeof(struct amp_engine_t));
	engine->rev = 1;
	engine->lock = sys_mutex_init(0);
	engine->sync = sys_mutex_init(0);
	engine->run = false;
	engine->core = amp_core_new(amp_audio_info(audio).rate);
	engine->clock = amp_basic_clock(amp_basic_new(120.0, 4.0, amp_audio_info(audio).rate));
	engine->instr = amp_instr_null;
	engine->comm = comm ?: amp_comm_new();
	engine->notify = amp_notify_new(list, notify, engine);
	engine->watch = NULL;
	engine->rt = (struct amp_rt_t){ engine, amp_engine_watch };

	ml_env_add(&engine->core->env, strdup("amp.rt"), ml_value_box(amp_box_ref(&engine->rt), ml_tag_copy(ml_tag_null)));

	iface = "unk";
	
#if ALSA
	if(audio.iface == &alsa_audio_iface)
		iface = "alsa";
#endif
	
#if PULSE
	if(audio.iface == &pulse_audio_iface)
		iface = "pulse";
#endif

	ml_env_add(&engine->core->env, strdup("amp.audio"), ml_value_str(strdup(iface), ml_tag_copy(ml_tag_null)));

		SF_INFO info;

		info.samplerate = amp_audio_info(audio).rate;
		info.channels = 1;
		info.format = SF_FORMAT_FLAC | SF_FORMAT_PCM_24;
		file = sf_open("/home/marc/test.flac", SFM_WRITE, &info);
		if(file == NULL)
			fatal("Failed to open file.");
	return engine;
}

/**
 * Delete an engine.
 *   @engine: The engine.
 */
void amp_engine_delete(struct amp_engine_t *engine)
{
	struct amp_watch_t *watch;

	sf_close(file);
	printf("close\n");
	while(engine->watch != NULL) {
		watch = engine->watch;
		engine->watch = watch->next;

		free(watch);
	}

	amp_notify_delete(engine->notify);
	amp_comm_delete(engine->comm);
	amp_clock_delete(engine->clock);
	amp_instr_erase(engine->instr);
	amp_core_delete(engine->core);
	sys_mutex_destroy(&engine->lock);
	sys_mutex_destroy(&engine->sync);
	free(engine);
}

/**
 * Add a watch to the engine.
 *   @engine: The engine.
 *   @func: The function.
 *   @arg: The argument.
 */
void amp_engine_watch(struct amp_engine_t *engine, amp_watch_f func, void *arg)
{
	struct amp_watch_t *watch;

	watch = malloc(sizeof(struct amp_watch_t));
	watch->func = func;
	watch->arg = arg;
	watch->next = engine->watch;

	engine->watch = watch;
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


void amp_engine_start(struct amp_engine_t *engine)
{
	struct amp_seek_t seek;

	engine->run = true;
	amp_clock_info(engine->clock, amp_info_start(&seek));
}

void amp_engine_stop(struct amp_engine_t *engine)
{
	struct amp_seek_t seek;

	engine->run = false;
	amp_clock_info(engine->clock, amp_info_stop(&seek));
}
