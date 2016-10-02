#include "common.h"


/*
 * local declarations
 */
static void encode(const char *output, const char *input);
static void decode(const char *output, const char *input);

static void noreturn error(const char *restrict format, ...);


/**
 * Main entry point.
 *   @argc: The number of arguments.
 *   @argv: The argumenet array.
 *   &returns: The exit code.
 */
int main(int argc, char **argv)
{
	int i;
	bool dec = false, args = true;
	const char *input = NULL, *output = NULL;

	for(i = 1; i < argc; i++) {
		if(args && (argv[i][0] == '-')) {
			if(argv[i][1] == '-') {
				if(argv[i][2] == '\0')
					args = false;
				else if(strcmp(argv[i] + 2, "decode") == 0)
					dec = true;
				else if(strcmp(argv[i] + 2, "output") == 0) {
					if(output != NULL)
						error("Output already specified.");
					else if(argv[++i] == NULL)
						error("Expected path after '--output'.");

					output = argv[i];
				}
				else if(memcmp(argv[i] + 2, "output=", 7) == 0) {
					if(output != NULL)
						error("Output already specified.");

					output = argv[i] + 9;
				}
				else
					error("Unknown flag '%s'.", argv[i]);
			}
			else {
				bool brk = false;
				char *ptr = argv[i] + 1;

				while(*ptr != '\0') {
					switch(*ptr) {
					case 'd':
						dec = true;
						break;

					case 'o':
						if(output != NULL)
							error("Output already specified.");

						ptr++;
						if(*ptr == '\0') {
							ptr = argv[++i];
							if(ptr == NULL)
								error("Expected path after '-o'.");
						}

						brk = true;
						output = ptr;

						break;

					default:
						error("Unknown flag '-%c'.", *ptr);
					}

					ptr++;
					if(brk)
						break;
				}
			}
		}
		else {
			if(input != NULL)
				error("Input already specified.\n");

			input = argv[i];
		}
	}

	if(input == NULL)
		error("Missing input path.");
	else if(output == NULL)
		error("Missing output path.");

	if(dec)
		decode(output, input);
	else
		encode(output, input);

	return 0;
}


/**
 * Encode a file.
 *   @output: The output.
 *   @input: The input.
 */
static void encode(const char *output, const char *input)
{
	int *dat, *arr;
	unsigned int i, shift;
	SNDFILE *file;
	SF_INFO info;
	struct acw_buf_t buf;

	info.format = 0;
	file = sf_open(input, SFM_READ, &info);
	if(file == NULL)
		error("Cannot read '%s'. %s", input, sf_strerror(NULL));

	switch(info.format & 0xF) {
	case SF_FORMAT_PCM_S8: shift = 24; break;
	case SF_FORMAT_PCM_16: shift = 16;; break;
	case SF_FORMAT_PCM_24: shift = 8;; break;
	case SF_FORMAT_PCM_32: shift = 8;; break;
	default: return error("Invalid format in '%s'. Expected signed integer format.", input);
	}

	dat = malloc(info.channels * info.frames * sizeof(int));
	sf_readf_int(file, dat, info.frames);

	arr = malloc(info.frames * sizeof(int));
	for(i = 0; i < info.frames; i++)
		arr[i] = dat[i * info.channels] >> shift;

	free(dat);
	sf_close(file);

	buf = acw_buf_enc32(arr, info.frames);
	buf.info.rate = info.samplerate;
	acw_buf_save(buf, output);
	acw_buf_delete(buf);

	free(arr);
}

/**
 * Decode a file.
 *   @output: The output.
 *   @input: The input.
 */
static void decode(const char *output, const char *input)
{
	fatal("stub");
}


/**
 * Print an error and exit.
 *   @format, ...: The printf-style format and arguments.
 *   &noreturn.
 */
static void noreturn error(const char *restrict format, ...)
{
	va_list args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	fprintf(stderr, "\n");
	va_end(args);

	exit(1);
}
