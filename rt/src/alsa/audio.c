#define _GNU_SOURCE
#include <alsa/asoundlib.h>
#include <pthread.h>
#include "../common.h"


/**
 * Audio configuration structure.
 *   @name, hack: The name and hack.
 *   @rate, in, out, period, nperiod: The stream parameters.
 */
struct alsa_conf_t {
	char name[32], hack[32];
	unsigned int rate, in, out, period, nperiod;
};

/**
 * Audio audio structure.
 *   @capture, playback: The PCMs.
 *   @pipe: The synchronization pipe.
 *   @thread: The thread.
 *   @conf: The configuration.
 *   @func: The audio function.
 *   @arg: The function argument.
 */
struct alsa_audio_t {
	snd_pcm_t *capture, *playback;
	snd_pcm_format_t format;

	int pipe[2];
	pthread_t thread;
	struct alsa_conf_t conf;

#ifdef PULSE
	struct pulse_hack_t *hack;
#endif

	amp_audio_f func;
	void *arg;
};


/*
 * local declarations
 */
static int audio_find(const char *id);
static void audio_conf(struct alsa_conf_t *conf, const char *str);

static void pcm_conf(snd_pcm_t *pcm, snd_pcm_stream_t stream, const struct alsa_conf_t *conf, snd_pcm_format_t *format);
static void *pcm_thread(void *arg);
static void pcm_prepare(struct alsa_audio_t *audio, void *raw);
static void pcm_drop(struct alsa_audio_t *device);
static unsigned short pcm_size(snd_pcm_format_t format);

static void pcm2float(double **restrict data, const uint8_t *restrict raw, snd_pcm_format_t format, unsigned int count, unsigned int chancnt);
static void float2pcm(uint8_t *restrict raw, double **restrict data, snd_pcm_format_t format, unsigned int count, unsigned int chancnt);

/*
 * global variables.
 */
const struct amp_audio_i alsa_audio_iface = {
	(amp_audio_open_f)alsa_audio_open,
	(amp_audio_close_f)alsa_audio_close,
	(amp_audio_exec_f)alsa_audio_exec,
	(amp_audio_halt_f)alsa_audio_halt,
	(amp_audio_info_f)alsa_audio_info
};

void unquote(const char **ptr, char *dest, unsigned int len)
{
	unsigned int i = 0;

	while(isspace(**ptr))
		(*ptr)++;

	if(**ptr == '\'') {
		(*ptr)++;

		while((**ptr != '\'') && (**ptr != '\0')) {
			if(i < len)
				dest[i++] = **ptr;

			(*ptr)++;
		}
		
		if(**ptr == '\'')
			(*ptr)++;
	}
	else {
		while(!isspace(**ptr) && (**ptr != '\0')) {
			if(i < len)
				dest[i++] = **ptr;

			(*ptr)++;
		}
	}

	dest[(i < len) ? i : (len - 1)] = '\0';
}


/**
 * Open an audio device.
 *   @conf: The configuration.
 *   &returns: The device or null on error.
 */
struct alsa_audio_t *alsa_audio_open(const char *conf)
{
	int err;
	struct alsa_audio_t *audio;

	audio = malloc(sizeof(struct alsa_audio_t));
	audio_conf(&audio->conf, conf);
	audio->func = NULL;
	audio->capture = NULL;
	audio->playback = NULL;

	if(audio->conf.in > 0) {
		err = snd_pcm_open(&audio->capture, audio->conf.name, SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK);
		if(err < 0)
			fprintf(stderr, "Cannot open device for capture. %s.\n", snd_strerror(err)), exit(1);

		pcm_conf(audio->capture, SND_PCM_STREAM_CAPTURE, &audio->conf, &audio->format);
	}

	if(audio->conf.out > 0) {
		err = snd_pcm_open(&audio->playback, audio->conf.name, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
		if(err < 0)
			fprintf(stderr, "Cannot open device for playback. %s.\n", snd_strerror(err)), exit(1);

		pcm_conf(audio->playback, SND_PCM_STREAM_PLAYBACK, &audio->conf, &audio->format);
	}

	if((audio->conf.in > 0) && (audio->conf.out > 0))
		snd_pcm_link(audio->playback, audio->capture);

	return audio;
}

/**
 * Close an audio device.
 *   @audio: The audio.
 */
void alsa_audio_close(struct alsa_audio_t *audio)
{
	if(audio->capture != NULL)
		snd_pcm_close(audio->capture);

	if(audio->playback != NULL)
		snd_pcm_close(audio->playback);

	free(audio);
}


/**
 * Find a audio client given an identifier.
 *   @id: The identifier.
 *   &returns: The client number if found, negative otherwise.
 */
static int audio_find(const char *id)
{
	char *name;
	bool match;
	int card = -1;

	while(true) {
		snd_card_next(&card);
		if(card < 0)
			break;

		snd_card_get_name(card, &name);
		match = (strcmp(name, id) == 0);
		_free(name);

		if(match)
			return card;
	}

	return -1;
}

/**
 * Parse an audio configuration.
 *   @conf: The configure.
 *   @str: The string.
 */
static void audio_conf(struct alsa_conf_t *conf, const char *str)
{
	int id;

	unquote(&str, conf->name, sizeof(conf->name));

	conf->hack[0] = '\0';
	conf->rate = 48000;
	conf->in = 2;
	conf->out = 2;
	conf->period = 256;
	conf->nperiod = 3;

	while(true) {
		long val;
		unsigned int *param;

		while(isspace(*str))
			str++;

		if(*str == '\0')
			break;

		if(*str == 'h') {
			str++;
			unquote(&str, conf->hack, sizeof(conf->hack));
		}
		else {
			switch(*str) {
			case 'r': param = &conf->rate; break;
			case 'i': param = &conf->in; break;
			case 'o': param = &conf->out; break;
			case 'p': param = &conf->period; break;
			case 'n': param = &conf->nperiod; break;
			default: fprintf(stderr, "Invalid ALSA option '%c'.\n", *str), exit(1);
			}

			str++;
			if(!isdigit(*str))
				fprintf(stderr, "Missing ALSA parameter.\n"), exit(1);

			errno = 0;
			val = strtol(str, (char **)&str, 0);
			if((errno != 0) || (val < 0) || (val > UINT_MAX))
				fprintf(stderr, "Invalid parameter. %s.\n", strerror(errno)), exit(1);

			*param = val;
		}
	}

	id = audio_find(conf->name);
	if(id >= 0)
		snprintf(conf->name, sizeof(conf->name), "hw:%d", id);
}


/**
 * Begin executing the ALSA audio device.
 *   @audio: The audio device.
 *   @func: The ready function.
 *   @arg: The argument.
 */

void alsa_audio_exec(struct alsa_audio_t *audio, amp_audio_f func, void *arg)
{
	int err;
	pthread_attr_t attr;
	struct sched_param param;

	if(audio->conf.hack[0] != '\0') {
#if PULSE
		audio->hack = pulse_hack_new(audio->conf.rate);
#else
		fprintf(stderr, "PulseAudio is not supported.\n"), exit(1);
#endif
	}
#if PULSE
	else
		audio->hack = NULL;
#endif

	if(audio->func != NULL)
		fprintf(stderr, "Device already running.\n"), exit(1);
	else if((audio->playback == NULL) && (audio->capture == NULL))
		fprintf(stderr, "Device not properly configured.\n"), exit(1);

	audio->func = func;
	audio->arg = arg;

	if(pipe(audio->pipe) < 0)
		fprintf(stderr, "Failed to create synchronization pipe. %s.\n", strerror(errno)), exit(1);

	err = pthread_attr_init(&attr);
	if(err != 0)
		fprintf(stderr, "Failed to initialize thread attributes. %s.\n", strerror(err)), exit(1);

	err = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	if(err != 0)
		fprintf(stderr, "Failed to set schedular inheritance. %s.\n", strerror(err)), exit(1);

	err = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	if(err != 0)
		fprintf(stderr, "Failed to set schedular policy. %s.\n", strerror(err)), exit(1);

	param.sched_priority = 99;
	err = pthread_attr_setschedparam(&attr, &param);
	if(err != 0)
		fprintf(stderr, "Failed to set schedular parameter. %s.\n", strerror(err)), exit(1);

	err = pthread_create(&audio->thread, &attr, pcm_thread, audio);
	if(err != 0) {
		printf("no RT\n");
		err = pthread_create(&audio->thread, NULL, pcm_thread, audio);
		if(err != 0)
			fprintf(stderr, "Failed to start thread. %s.\n", strerror(err)), exit(1);
	}

	pthread_attr_destroy(&attr);
	if(err != 0)
		fprintf(stderr, "Failed to destroy thread attributes. %s.\n", strerror(err)), exit(1);
}

/**
 * Halt the ALSA audio device.
 *  @audio: The audio device.
 */
void alsa_audio_halt(struct alsa_audio_t *audio)
{
	int err;
	uint8_t cmd;

	if(audio->func == NULL)
		fprintf(stderr, "Device not running.\n"), exit(1);

	cmd = 1;
	err = write(audio->pipe[1], &cmd, 1);
	if(err < 0)
		fprintf(stderr, "Failed to write to descriptor. %s.\n", strerror(errno)), exit(1);

	pthread_join(audio->thread, NULL);

#if PULSE
	if(audio->hack != NULL)
		pulse_hack_delete(audio->hack);
#endif

	audio->func = NULL;
	close(audio->pipe[0]);
	close(audio->pipe[1]);
}


/**
 * Retrieve audio information.
 *   @audio: The audio device.
 *   &returns: The information structure.
 */
struct amp_audio_info_t alsa_audio_info(struct alsa_audio_t *audio)
{
	return (struct amp_audio_info_t){ audio->conf.rate };
}


/**
 * Configure a PCM device.
 *   @pcm: The PCM device.
 *   @stream: The stream type.
 *   @conf: The configuration paratemers.
 *   @format: The configured format.
 */
static void pcm_conf(snd_pcm_t *pcm, snd_pcm_stream_t stream, const struct alsa_conf_t *conf, snd_pcm_format_t *format)
{
	static snd_pcm_format_t formatlist[] = {
		//SND_PCM_FORMAT_S24,
		//SND_PCM_FORMAT_S24_LE,
		SND_PCM_FORMAT_S24_3LE,
		//SND_PCM_FORMAT_S24_BE,
		//SND_PCM_FORMAT_S24_3BE,
		//SND_PCM_FORMAT_S32,
		//SND_PCM_FORMAT_S32_LE,
		//SND_PCM_FORMAT_S32_BE,
		//SND_PCM_FORMAT_S16,
		//SND_PCM_FORMAT_S16_LE,
		//SND_PCM_FORMAT_S16_BE,
		//SND_PCM_FORMAT_S8,
		SND_PCM_FORMAT_UNKNOWN
	};

	int err;
	unsigned int i, val;
	snd_pcm_uframes_t valf;
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_sw_params_t *sw_params;

	snd_pcm_sw_params_alloca(&sw_params);
	snd_pcm_hw_params_alloca(&hw_params);

	err = snd_pcm_hw_params_any(pcm, hw_params);
	if(err < 0)
		fprintf(stderr, "Failed to retrieved hardware parameters. %s.\n", snd_strerror(err));

	for(i = 0; formatlist[i] != SND_PCM_FORMAT_UNKNOWN; i++) {
		err = snd_pcm_hw_params_set_format(pcm, hw_params, formatlist[i]);
		if(err == 0)
			break;
	}
	
	if(formatlist[i] == SND_PCM_FORMAT_UNKNOWN)
		fprintf(stderr, "Failed find appropriate hardware format. %s.\n", snd_strerror(err)), exit(1);

	*format = formatlist[i];
	err = snd_pcm_hw_params_set_access(pcm, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
	if(err < 0)
		fprintf(stderr, "Failed to set interlieved access. %s.\n", snd_strerror(err)), exit(1);
	
	val = conf->rate;
	err = snd_pcm_hw_params_set_rate_near(pcm, hw_params, &val, 0);
	if(err < 0)
		fprintf(stderr, "Invalid sample rate '%u'. %s.\n", conf->rate, snd_strerror(err)), exit(1);
	
	if(val != conf->rate)
		fprintf(stderr, "Invalid sample rate '%u'. %s.\n", conf->rate, snd_strerror(err)), exit(1);
	
	if(stream == SND_PCM_STREAM_PLAYBACK)
		val = conf->out;
	else
		val = conf->in;
	
	err = snd_pcm_hw_params_set_channels(pcm, hw_params, val);
	if(err < 0)
		fprintf(stderr, "Invalid number of %s channels. %s.\n", stream == SND_PCM_STREAM_PLAYBACK ? "playback" : "capture", snd_strerror(err)), exit(1);
	
	valf = conf->nperiod * conf->period;
	err = snd_pcm_hw_params_set_buffer_size_near(pcm, hw_params, &valf);
	if(err < 0)
		fprintf(stderr, "Unable to set buffer size '%lu'. %s.\n", valf, snd_strerror(err)), exit(1);
	
	if(valf != (conf->nperiod * conf->period))
		fprintf(stderr, "Unable to set buffer size '%lu'.\n", valf), exit(1);
	
	valf = conf->period;
	err = snd_pcm_hw_params_set_period_size_near(pcm, hw_params, &valf, 0);
	if(err < 0)
		fprintf(stderr, "Unable to set period size '%u'. %s.\n", conf->period, snd_strerror(err)), exit(1);
	
	if(valf != conf->period)
		fprintf(stderr, "Unable to set period size '%u'. Suggested '%lu'.\n", conf->period, valf), exit(1);
	
	err = snd_pcm_hw_params(pcm, hw_params);
	if(err < 0)
		fprintf(stderr, "Failed to set hardware parameters. %s.\n", snd_strerror(err)), exit(1);

	err = snd_pcm_sw_params_current(pcm, sw_params);
	if(err < 0)
		fprintf(stderr, "Failed find appropriate software format. %s.\n", snd_strerror(err)), exit(1);
	
	err = snd_pcm_sw_params_set_start_threshold(pcm, sw_params, 0x7FFFFFFF);
	if(err < 0)
		fprintf(stderr, "Unable to set start threshold. %s.\n", snd_strerror(err)), exit(1);
	
	err = snd_pcm_sw_params_set_avail_min(pcm, sw_params, 4);
	if(err < 0)
		fprintf(stderr, "Unable to set available minimum. %s.\n", snd_strerror(err)), exit(1);
	
	err = snd_pcm_sw_params(pcm, sw_params);
	if(err < 0)
		fprintf(stderr, "Unable to set software parameters. %s.\n", snd_strerror(err)), exit(1);
}

/**
 * The PCM device thread.
 *   @arg: The device argument.
 *   &returns: Always 'NULL'.
 */
static void *pcm_thread(void *arg)
{
	int err;
	struct alsa_audio_t *audio = arg;
	const struct alsa_conf_t *conf = &audio->conf;
	unsigned short sz = pcm_size(audio->format);
	unsigned int i, chan = (conf->in > conf->out) ? conf->in : conf->out;
	uint8_t raw[sz * chan * conf->period];
	unsigned int nfds = ((conf->in > 0) ? snd_pcm_poll_descriptors_count(audio->capture) : 0) + 1;
	struct pollfd fdset[nfds];
	double data[chan][conf->period], *buf[chan];

	for(i = 0; i < chan; i++)
		buf[i] = data[i];

	if(audio->conf.in > 0)
		snd_pcm_poll_descriptors(audio->capture, fdset, nfds-1);

	fdset[nfds-1].fd = audio->pipe[0];
	fdset[nfds-1].events = POLLIN;

	pcm_prepare(audio, raw);

	while(true) {
		unsigned short revents;

		for(i = 0; i < nfds; i++)
			fdset[i].revents = 0;

		poll(fdset, nfds, -1);

		if(fdset[nfds-1].revents & POLLIN) {
			int err;
			uint8_t cmd;

			err = read(audio->pipe[0], &cmd, 1);
			if(err < 0)
				fprintf(stderr, "Failed to read from sychronization pipe. %s.\n", strerror(errno)), exit(1);

			break;
		}

		snd_pcm_poll_descriptors_revents(audio->capture, fdset, nfds-1, &revents);
		if(revents & (POLLIN | POLLERR)) {
			snd_pcm_sframes_t count = 0;

			if(audio->capture != NULL) {
				do
					count = snd_pcm_readi(audio->capture, raw, audio->conf.period);
				while(count == -EAGAIN);
			}

			if(count == -EPIPE) {
				audio->func(NULL, 0, audio->arg);

				pcm_drop(audio);
				pcm_prepare(audio, raw);

				continue;
			}

			if(count < 0)
				fprintf(stderr, "Failed read. %s.\n", snd_strerror(count)), exit(1);

			pcm2float(buf, raw, audio->format, count, chan);

			audio->func(buf, count, audio->arg);
			//dsp_zero_d(buf[0], count);
			//dsp_zero_d(buf[1], count);
			dsp_clamp_d(buf[0], count);
			dsp_clamp_d(buf[1], count);

#if PULSE
			if(audio->hack != NULL)
				pulse_hack_proc(audio->hack, buf, count);
#endif

			float2pcm(raw, buf, audio->format, count, chan);

			err = snd_pcm_writei(audio->playback, raw, count);
			if(err == -EPIPE) {
				audio->func(NULL, 0, audio->arg);

				pcm_drop(audio);
				pcm_prepare(audio, raw);

				continue;
			}
		}
	}

	pcm_drop(audio);

	return NULL;
}

/**
 * Prepare data on a PCM device.
 *   @audio: The audio device.
 */
static void pcm_prepare(struct alsa_audio_t *audio, void *raw)
{
	int err;
	unsigned int i;

	if(audio->playback != NULL) {
		err = snd_pcm_prepare(audio->playback);
		if(err < 0)
			fprintf(stderr, "Error preparing playback audio device. %s.\n", snd_strerror(err)), exit(1);
	}

	if(audio->capture != NULL) {
		err = snd_pcm_prepare(audio->capture);
		if(err < 0)
			fprintf(stderr, "Error preparing playback audio device. %s.\n", snd_strerror(err)), exit(1);
	}

	if(audio->playback != NULL) {
		snd_pcm_format_set_silence(audio->format, raw, audio->conf.period * audio->conf.out);

		for(i = 0; i < audio->conf.nperiod; i++) {
			err = snd_pcm_writei(audio->playback, raw, audio->conf.period);
			if(err < 0)
				fprintf(stderr, "Unable to write to ALSA audio device. %s.\n", snd_strerror(err)), exit(1);
		}
	}

	err = snd_pcm_start(audio->capture ?: audio->playback);
	if(err < 0)
		fprintf(stderr, "Error starting ALSA audio device. %s.\n", snd_strerror(err)), exit(1);
}

/**
 * Drop data on a PCM device.
 *   @device: The audio device.
 */
static void pcm_drop(struct alsa_audio_t *device)
{
	int err;

	if(device->capture != NULL) {
		err = snd_pcm_drop(device->capture);
		if(err < 0)
			fprintf(stderr, "Error clearing ALSA capture device.\n"), exit(1);
	}

	if(device->playback != NULL) {
		err = snd_pcm_drop(device->playback);
		if(err < 0)
			fprintf(stderr, "Error clearing ALSA playback device.\n"), exit(1);
	}
}

/**
 * Retrieves the size of each data element of a format.
 *   @format: The given format type.
 *   &returns: The size of a single data element.
 */
static unsigned short pcm_size(snd_pcm_format_t format)
{
	switch(format) {
	case SND_PCM_FORMAT_S24_LE:
	case SND_PCM_FORMAT_S24_BE:
	case SND_PCM_FORMAT_S32_LE:
	case SND_PCM_FORMAT_S32_BE:
		return 4;

	case SND_PCM_FORMAT_S24_3LE:
	case SND_PCM_FORMAT_S24_3BE:
		return 3;

	case SND_PCM_FORMAT_S16_LE:
	case SND_PCM_FORMAT_S16_BE:
		return 2;

	case SND_PCM_FORMAT_S8:
		return 1;

	default:
		return 0;
	}
}


/**
 * Convert raw PCM values into double-precision floating point data.
 *   @data: The buffer to contain the floating point values.
 *   @raw: The raw PCM data.
 *   @format: The PCM format.
 *   @count: The number of values.
 *   @chancnt: The number of channels.
 */

static void pcm2float(double **restrict data, const uint8_t *restrict raw, snd_pcm_format_t format, unsigned int count, unsigned int chancnt)
{
	unsigned int i, ii;

	switch(format) {
	case SND_PCM_FORMAT_S16:
		{
			const int16_t *ptr = (const int16_t *)raw;

			for(ii = chancnt - 1; ii != (unsigned int )-1; ii--) {
				for(i = count - 1; i != (unsigned int)-1; i--)
					data[ii][i] = ptr[chancnt*i + ii] / (double)INT16_MAX;
			}
		}
		break;

	case SND_PCM_FORMAT_S24:
		{
			const int32_t *ptr = (const int32_t *)raw;

			for(ii = chancnt - 1; ii != (unsigned int )-1; ii--) {
				for(i = count - 1; i != (unsigned int)-1; i--)
					data[ii][i] = ptr[chancnt*i + ii] / (double)((INT32_MAX >> 8) - 1);
			}
		}
		break;

	case SND_PCM_FORMAT_S32:
		{
			const int32_t *ptr = (const int32_t *)raw;

			for(ii = chancnt - 1; ii != (unsigned int )-1; ii--) {
				for(i = count - 1; i != (unsigned int)-1; i--)
					data[ii][i] = ptr[chancnt*i + ii] / (double)(INT32_MAX - 1);
			}
		}
		break;

	case SND_PCM_FORMAT_S24_3LE:
		{
			int32_t val;
			unsigned int index;

			for(ii = chancnt - 1; ii != (unsigned int )-1; ii--) {
				for(i = count - 1; i != (unsigned int)-1; i--) {
					val = 0;
					index = chancnt * i + ii;

					val |= raw[3*index + 0] << 0;
					val |= raw[3*index + 1] << 8;
					val |= raw[3*index + 2] << 16;

					if(val & 0x00800000)
						val |= 0xFF000000;

					data[ii][i] = (double)val / (double)0x00800000;
				}
			}
		}
		break;
		
	default:
		fprintf(stderr, "Unsupported format: %d\n", format), exit(1);
	}
}

/**
 * Converts double-precision floating point data to raw PCM values.
 *   @raw: A pointer to the raw output buffer.
 *   @data: The floating point input.
 *   @format: The PCM format.
 *   @count: The number of values.
 *   @chancnt: The number of channels.
 */

static void float2pcm(uint8_t *restrict raw, double **restrict data, snd_pcm_format_t format, unsigned int count, unsigned int chancnt)
{
	unsigned int i, ii;

	switch(format) {
	case SND_PCM_FORMAT_S16:
		{
			int16_t *ptr = (int16_t *)raw;

			for(ii = chancnt - 1; ii != (unsigned int)-1; ii--) {
				for(i = count - 1; i != (unsigned int)-1; i--)
					ptr[chancnt*i + ii] = data[ii][i] * INT16_MAX;
			}
		}
		break;

	case SND_PCM_FORMAT_S24:
		{
			uint32_t *ptr = (uint32_t *)raw;

			for(ii = chancnt - 1; ii != (unsigned int)-1; ii--) {
				for(i = count - 1; i != (unsigned int)-1; i--)
					ptr[chancnt*i + ii] = data[ii][i] * ((INT32_MAX >> 8) - 1);
			}
		}
		break;

	case SND_PCM_FORMAT_S32:
		{
			int32_t *ptr = (int32_t *)raw;

			for(ii = chancnt - 1; ii != (unsigned int)-1; ii--) {
				for(i = count - 1; i != (unsigned int)-1; i--)
					ptr[chancnt*i + ii] = data[ii][i] * (INT32_MAX - 1);
			}
		}
		break;

	case SND_PCM_FORMAT_S24_3LE:
		{
			int32_t val;
			unsigned int index;

			for(ii = chancnt - 1; ii != (unsigned int)-1; ii--) {
				for(i = count - 1; i != (unsigned int)-1; i--) {
					index = chancnt * i + ii;
					val = data[ii][i] * 0x007FFFFF;

					raw[3*index + 0] = (val & 0x0000FF) >> 0;
					raw[3*index + 1] = (val & 0x00FF00) >> 8;
					raw[3*index + 2] = (val & 0xFF0000) >> 16;
				}
			}
		}
		break;
		
	default:
		fprintf(stderr, "Unsupported format: %d\n", format), exit(1);
	}
}
