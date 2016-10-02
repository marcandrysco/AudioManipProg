#include "common.h"


/**
 * Load a buffer.
 *   @buf: Ref. The buffer.
 *   @path: The path.
 *   &returns: Error.
 */
char *acw_buf_load(struct acw_buf_t *buf, const char *path)
{
#define onexit fclose(file);
	FILE *file;
	size_t nbytes;

	file = fopen(path, "r");
	if(file == NULL)
		return mprintf("Unable to open '%s'. %s.", path, strerror(errno));

	if(fread(&buf->info, sizeof(struct acw_info_t), 1, file) < 1)
		fail("Failed to read from '%s'. %s.", path, strerror(errno));

	if(buf->info.magic != ACW_MAGIC_1_0)
		fail("Invalid file '%s'. Expected ACW.", path);

	buf->raw = malloc(nbytes = buf->info.nbytes);
	if(fread(buf->raw, 1, nbytes, file) < nbytes)
		fail("Failed to read from '%s'. %s.", path, strerror(errno));

	fclose(file);

	return NULL;
#undef onexit
}

/**
 * Save the file to a buffer.
 *   @buf: The buffer.
 *   @path: The path.
 *   &returns: Error.
 */
char *acw_buf_save(struct acw_buf_t buf, const char *path)
{
	FILE *file;

	file = fopen(path, "w");
	if(file == NULL)
		return mprintf("Unable to open '%s' for writing.");

	fwrite(&buf.info, 1, sizeof(struct acw_info_t), file);
	fwrite(buf.raw, buf.info.nbytes, 1, file);
	fclose(file);

	return NULL;
}

/**
 * Delete a buffer.
 *   @buf: The buffer.
 */
void acw_buf_delete(struct acw_buf_t buf)
{
	free(buf.raw);
}


/**
 * Directly encode from a 32-bit buffer.
 *   @arr: The array.
 *   @len: The length.
 *   &returns: The buffer.
 */
struct acw_buf_t acw_buf_enc32(int32_t *arr, unsigned int len)
{
	struct acw_encode_t *enc;

	enc = acw_encode_new();
	acw_encode_proc(enc, arr, len);

	return acw_encode_done(enc);
}


/**
 * Read PCM data from a buffer.
 *   @buf: The buffer.
 *   &returns: The PCM array.
 */
int32_t *acw_buf_pcm32(struct acw_buf_t buf)
{
	int32_t *arr;
	unsigned int i;
	struct acw_play_t play;

	play = acw_play_init(buf.raw);
	arr = malloc(sizeof(int32_t) * buf.info.length);

	for(i = 0; i < buf.info.length; i++)
		arr[i] = acw_play_next(&play);

	return arr;
}


/**
 * Load raw data from the path.
 *   @path: The path.
 *   @info: The information.
 *   &returns: The data or null.
 */
void *acw_load_raw(const char *path, struct acw_info_t *info)
{
	void *raw;
	FILE *file;
	struct acw_info_t sinfo;

	if(info == NULL)
		info = &sinfo;

	file = fopen(path, "r");
	if(file == NULL)
		return NULL;

	if(fread(info, sizeof(struct acw_info_t), 1, file) < 1)
		return fclose(file), NULL;

	raw = malloc(info->nbytes);

	if(fread(raw, 1, info->nbytes, file) < info->nbytes)
		return fclose(file), NULL;

	fclose(file);

	return raw;
}
