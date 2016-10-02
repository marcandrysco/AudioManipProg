#include "common.h"


/**
 * Main entry point.
 *   @argc: The number of arguments.
 *   @argv: The argumenet array.
 *   &returns: The exit code.
 */
int main(int argc, char **argv)
{
	unsigned int i;
	int *arr, *cmp;
	SNDFILE *file;
	SF_INFO info;
	struct acw_buf_t buf;
	const char *path = "test.flac";

	info.format = 0;
	file = sf_open(path, SFM_READ, &info);
	if(file == NULL)
		fatal("Cannot file test file '%s'.", path);

	if(info.channels != 1)
		fatal("Test file must be mono.");

	arr = malloc(info.frames * sizeof(int));
	sf_readf_int(file, arr, info.frames);
	sf_close(file);

	for(i = 0; i < info.frames; i++)
		arr[i] >>= 8;

	buf = acw_buf_enc32(arr, info.frames);
	cmp = acw_buf_pcm32(buf);

	if(buf.info.length != info.frames)
		fatal("Input and ACW differ on length.");

	for(i = 0; i < info.frames; i++) {
		if(arr[i] != cmp[i])
			fatal("Input and ACW differ on input #%u.\n", i);
	}

	free(arr);
	free(cmp);
	acw_buf_delete(buf);

	if(hax_memcnt > 0)
		fprintf(stderr, "missing %d allocations\n", hax_memcnt), exit(1);

	printf("success\n");

	return 0;
}
