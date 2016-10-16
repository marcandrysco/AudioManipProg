#include "common.h"


/*
 * local declarations
 */
static char *optlong(char ***arg, const char *opt);


/**
 * Create a string list.
 *   &returns: The list.
 */
char **strlist_new(void)
{
	char **list;

	list = malloc(sizeof(char *));
	list[0] = NULL;

	return list;
}

/**
 * Add a string to the string list.
 *   @list: Ref. The string list reference.
 *   @str: The new string.
 */
void strlist_add(char ***list, char *str)
{
	unsigned int i;

	i = 0;
	while((*list)[i] != NULL)
		i++;

	*list = realloc(*list, (i + 2) * sizeof(void *));
	(*list)[i] = str;
	(*list)[i+1] = NULL;
}

/**
 * Delete a string list.
 *   @list: The string list.
 */
void strlist_delete(char **list)
{
	unsigned int i;

	for(i = 0; list[i] != NULL; i++)
		free(list[i]);

	free(list);
}


/**
 * Main entry point.
 *   @argc: The number of arguments.
 *   @argv: The argument array.
 *   &returns: Always zero.
 */
int main(int argc, char **argv)
{
	struct amp_audio_t audio;
	struct amp_comm_t *comm;
	const struct amp_audio_i *iface = NULL;
	char **arg, *file = NULL, **plugin, *val, *conf = NULL;

	srand(sys_utime());

	plugin = strlist_new();
	comm = amp_comm_new();

	for(arg = argv + 1; *arg != NULL; arg++) {
		if((*arg)[0] != '-') {
			if(file != NULL)
				fprintf(stderr, "Cannot specify two files."), exit(1);

			file = *arg;
		}
		else if((*arg)[1] == '-') {
			if((val = optlong(&arg, "--plugin")) != NULL)
				strlist_add(&plugin, strdup(val));
			else if((val = optlong(&arg, "--dummy")) != NULL) {
				if(iface != NULL)
					fprintf(stderr, "Cannot specify multiple audio interfaces.\n"), exit(1);

				conf = val;
				iface = &dummy_audio_iface;
			}
			else if((val = optlong(&arg, "--alsa")) != NULL) {
				if(iface != NULL)
					fprintf(stderr, "Cannot specify multiple audio interfaces.\n"), exit(1);

#if ALSA
				conf = val;
				iface = &alsa_audio_iface;
#else
				fprintf(stderr, "ALSA is unavailable.\n"), exit(1);
#endif
			}
			else if((val = optlong(&arg, "--pulse")) != NULL) {
				if(iface != NULL)
					fprintf(stderr, "Cannot specify multiple audio interfaces.\n"), exit(1);

#if PULSE
				conf = val;
				iface = &pulse_audio_iface;
#else
				fprintf(stderr, "PulseAudio is unavailable.\n"), exit(1);
#endif
			}
			else if((val = optlong(&arg, "--midi")) != NULL) {
				const char *str;
				unsigned long dev;
				struct amp_midi_i *iface;

				switch(val[0]) {
				case 'a':
#if ALSA
					iface = &alsa_midi_iface;
#else
					fprintf(stderr, "ALSA is unavailable.\n"), exit(1);
#endif
					break;

				default:
					fprintf(stderr, "Invalid MIDI interface '%c'.\n", val[0]), exit(1);
				}

				str = val + 1;
				if(isdigit(*str)) {
					errno = 0;
					dev = strtoul(str, (char **)&str, 0);
					if((errno != 0) || (dev > UINT16_MAX))
						fprintf(stderr, "Invalid device number.\n"), exit(1);
				}
				else
					dev = 0;

				while(isspace(*str))
					str++;

				if(*str == '\0')
					fprintf(stderr, "MIDI device missing identifier.\n"), exit(1);

				amp_comm_add(comm, dev, str, iface);
			}
			else
				fprintf(stderr, "Unknown option '%s'.\n", *arg), exit(1);
		}
		else
			fprintf(stderr, "Cannot handle short options.\n"), exit(1);
	}

	if(iface == NULL)
		fprintf(stderr, "No audio interface selected.\n"), exit(1);
	if(file == NULL)
		fprintf(stderr, "Missing source file.\n"), exit(1);

	audio = amp_audio_open(conf, iface);
	amp_exec(audio, file, plugin, comm);
	amp_audio_close(audio);
	strlist_delete(plugin);

	if(hax_memcnt != 0)
		fprintf(stderr, "allocated memory: %d\n", hax_memcnt);

	return 0;
}

/**
 * Retrieve a long option.
 *   @arg: The argument pointer.
 *   @opt: The option.
 *   &returns: The value if matched, false otherwise.
 */
static char *optlong(char ***arg, const char *opt)
{
	unsigned int len = strlen(opt);

	if(memcmp(**arg, opt, len) != 0)
		return NULL;

	if((**arg)[len] == '=') {
		return **arg + len + 1;
	}
	else if((**arg)[len] == '\0') {
		(*arg)++;
		if(**arg == NULL)
			fprintf(stderr, "Option '%s' expected parameter.\n", opt), exit(1);

		return **arg;
	}
	else
		return NULL;
}
