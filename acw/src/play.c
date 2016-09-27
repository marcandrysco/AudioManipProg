#include "common.h"


/**
 * Load a buffer.
 *   @path: The path.
 *   &returns: The buffer or null.
 */
struct acw_buf_t *acw_buf_load(const char *path)
{
	FILE *file;
	struct acw_info_t info;
	struct acw_buf_t *buf;

	file = fopen(path, "r");
	if(file == NULL)
		return NULL;

	if(fread(&info, sizeof(info), 1, file) < 1)
		return fclose(file), NULL;

	buf = malloc(sizeof(struct acw_buf_t));
	buf->info = info;
	buf->data = malloc(info.nbytes);

	if(fread(buf->data, 1, info.nbytes, file) < info.nbytes)
		return fclose(file), NULL;

	fclose(file);

	return buf;
}

/**
 * Delete a buffer.
 *   @buf: The buffer.
 */
void acw_buf_delete(struct acw_buf_t *buf)
{
	free(buf->data);
	free(buf);
}


/**
 * Read PCM data from a buffer.
 *   @buf: The buffer.
 *   &returns: The PCM array.
 */
int32_t *acw_buf_pcm32(struct acw_buf_t *buf)
{
	int32_t *arr;
	unsigned int i;
	struct acw_play_t play;

	play = acw_play_init(buf->data);
	arr = malloc(sizeof(int32_t) * buf->info.ndata);

	for(i = 0; i < buf->info.ndata; i++)
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
