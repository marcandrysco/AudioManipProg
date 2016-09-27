#include "common.h"


/**
 * Main entry point.
 *   @argc: The number of arguments.
 *   @argv: The argumenet array.
 *   &returns: The exit code.
 */
int main(int argc, char **argv)
{
	struct acw_encode_t *enc;

	enc = acw_encode_new(5);
	acw_encode_proc(enc, 1);
	acw_encode_proc(enc, 2);
	acw_encode_done(enc);

	if(hax_memcnt > 0)
		fprintf(stderr, "missing %d allocations\n", hax_memcnt), exit(1);
	/*
	struct acw_buf_t *buf;
	int32_t *pcm;

	buf = acw_buf_load("piano1.acw");
	assert(buf != NULL);

	pcm = acw_buf_pcm32(buf);

	SNDFILE *file;
	SF_INFO info;

	info.samplerate = 44100;
	info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_24;
	info.channels = 1;

	file = sf_open("piano1_ref.wav", SFM_WRITE, &info);
	assert(file != NULL);

	for(int i = 0; i < buf->info.ndata; i++)
		pcm[i] *= 256;

	sf_writef_int(file, pcm, buf->info.ndata);

	sf_close(file);

	acw_buf_delete(buf);

	{
		SNDFILE *left, *right;
		SF_INFO linfo, rinfo;

		linfo.format = rinfo.format = 0;
		left = sf_open("piano1.flac", SFM_READ, &linfo);
		right = sf_open("piano1_ref.wav", SFM_READ, &rinfo);

		assert((left != NULL) && (right != NULL));
		//assert(linfo.format == rinfo.format);
		assert(linfo.samplerate == rinfo.samplerate);
		assert(linfo.channels == rinfo.channels);
		assert(linfo.frames == rinfo.frames);

		int *lval, *rval;

		lval = malloc(linfo.frames * sizeof(int));
		rval = malloc(rinfo.frames * sizeof(int));

		sf_readf_int(left, lval, linfo.frames);
		sf_readf_int(right, rval, rinfo.frames);

		for(int i = 0; i < linfo.frames; i++)
			assert(lval[i] == rval[i]);

		free(lval);
		free(rval);

		sf_close(left);
		sf_close(right);
	}
	*/

	return 0;
}
