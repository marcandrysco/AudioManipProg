#include "common.h"

/**
 * File cache.
 *   @file: The file.
 */
struct amp_cache_t {
	struct amp_file_t *file;
};

/**
 * File structure.
 *   @path: The path.
 *   @chan: The channel.
 *   @buf: The buffer.
 *   @refcnt: The reference count.
 *   @cache: The cache.
 *   @next: The next file.
 */
struct amp_file_t {
	char *path;
	unsigned int chan;
	struct acw_buf_t buf;

	struct amp_cache_t *cache;
	unsigned int refcnt;

	struct amp_file_t *next;
};


/**
 * Create a new cache.
 *   @cache: The cache.
 */
struct amp_cache_t *amp_cache_new(void)
{
	struct amp_cache_t *cache;

	cache = malloc(sizeof(struct amp_cache_t));
	cache->file = NULL;

	return cache;
}

/**
 * Delete a cache.
 *   @cache: The cache.
 */
void amp_cache_delete(struct amp_cache_t *cache)
{
	struct amp_file_t *cur, *next;

	for(cur = cache->file; cur != NULL; cur = next) {
		next = cur->next;

		acw_buf_delete(cur->buf);
		free(cur->path);
		free(cur);
	}

	free(cache);
}


/**
 * Look up a file in the cache.
 *   @cache: The cache.
 *   @path: The path.
 *   @chan: The channel.
 *   &returns: The file if successful, false otherwise.
 */
struct amp_file_t *amp_cache_lookup(struct amp_cache_t *cache, const char *path, unsigned int chan)
{
	struct amp_file_t *file;

	for(file = cache->file; file != NULL; file = file->next) {
		if((strcmp(file->path, path) == 0) && (file->chan == chan))
			return file;
	}

	return NULL;
}


/**
 * Open a file in the cache.
 *   @cache: The cache.
 *   @path: The path.
 *   @chan: The channel.
 *   @rate: The sample rate.
 *   &returns: The file if successful, false otherwise.
 */
struct amp_file_t *amp_cache_open(struct amp_cache_t *cache, const char *path, unsigned int chan, unsigned int rate)
{
	struct amp_file_t *file;

	file = amp_cache_lookup(cache, path, chan);
	if(file == NULL) {
		struct acw_buf_t buf;

		if(!chkbool(acw_buf_load(&buf, path)))
			return NULL;

		file = malloc(sizeof(struct amp_file_t));
		file->refcnt = 1;
		file->buf = buf;
		file->cache = cache;
		file->chan = chan;
		file->path = strdup(path);
		file->next = cache->file;
		cache->file = file;
	}
	else
		file->refcnt++;

	return file;
}

/**
 * Close a file, removing it from the cache.
 *   @cache: The cache.
 *   @file: The file.
 */
void amp_cache_close(struct amp_cache_t *cache, struct amp_file_t *file)
{
	struct amp_file_t **ptr;

	for(ptr = &cache->file; *ptr != NULL; *ptr = (*ptr)->next) {
		if(*ptr != file)
			continue;

		*ptr = (*ptr)->next;
		break;
	}

	acw_buf_delete(file->buf);
	free(file->path);
	free(file);
}


/**
 * Copy a file by adding a reference.
 *   @file: The file.
 *   &returns: The copy.
 */
struct amp_file_t *amp_file_copy(struct amp_file_t *file)
{
	file->refcnt++;

	return file;
}

/**
 * Delete a reference from a file.
 *   @file: The file.
 */
void amp_file_delete(struct amp_file_t *file)
{
	if(--file->refcnt == 0)
		amp_cache_close(file->cache, file);
}


/**
 * Retrieve the buffer from a file.
 *   @file: The file.
 *   &returns: The buffer.
 */
struct acw_buf_t amp_file_buf(struct amp_file_t *file)
{
	return file->buf;
}


#if 0
/**
 * Open a new buffer.
 *   @path: The file path.
 *   @chan: The channel.
 *   &returns: The buffer.
 */
struct amp_buf_t amp_buf_new(const char *path, unsigned int chan)
{
	static uint64_t tm = 0, last = 0;
	static uint64_t len = 1;

	if(last > 0) {
		tm += sys_utime() - last;
		printf("%d\n", tm / len++);
	}
	last = sys_utime();
	return (struct amp_buf_t){ amp_buf_arr_v, { .arr = amp_arr_new(path, chan) } };
}

/**
 * Delete a buffer.
 *   @buf: The buffer.
 */
void amp_buf_delete(struct amp_buf_t buf)
{
	switch(buf.type) {
	case amp_buf_arr_v: amp_arr_delete(buf.data.arr); break;
	}
}


/**
 * Create a new array.
 *   @path: The path.
 *   @chan: The channel.
 *   &returns: The array.
 */
#include <sys/stat.h>
struct amp_arr_t *amp_arr_new(const char *path, unsigned int chan)
{
	SNDFILE *file;
	SF_INFO info;
	int *raw;
	unsigned int i;
	struct amp_arr_t *arr;

	info.format = 0;
	file = sf_open(path, SFM_READ, &info);
	if(file == NULL)
		return NULL;

	if(info.channels <= chan)
		return sf_close(file), NULL;

	raw = malloc(info.channels * info.frames * sizeof(int));

	//sf_readf_int(file, raw, info.frames);
	sf_close(file);

	FILE *fp = fopen(path, "r");
	unsigned int rd = 0;
	while(!feof(fp))
		rd += fread(raw, 1, 10*1024, fp);
	struct stat st;
	stat(path, &st);
	printf("rd: %d %d\n", rd, st.st_size);
	fclose(fp);

	arr = malloc(sizeof(struct amp_arr_t));
	arr->len = info.frames;
	arr->rate = info.samplerate;
	arr->arr = malloc(3 * arr->len);

	for(i = 0; i < arr->len; i++)
		;//*(uint32_t *)(arr->arr + i) = (raw[i * info.channels + chan] >> 8) & 0x00FFFFFF;

	free(raw);

	return arr;
}

/**
 * Delete a array.
 *   @arr: The array.
 */
void amp_arr_delete(struct amp_arr_t *arr)
{
	free(arr->arr);
	free(arr);
}
#endif
