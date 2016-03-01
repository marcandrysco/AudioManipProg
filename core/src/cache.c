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
	struct dsp_buf_t *buf;

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

		dsp_buf_delete(cur->buf);
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
 *   &returns: The file if successful, false otherwise.
 */

struct amp_file_t *amp_cache_open(struct amp_cache_t *cache, const char *path, unsigned int chan)
{
	struct amp_file_t *file;

	file = amp_cache_lookup(cache, path, chan);
	if(file == NULL) {
		struct dsp_buf_t *buf;

		buf = dsp_buf_load(path, chan, 48000);
		if(buf == NULL)
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

	dsp_buf_delete(file->buf);
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
	amp_file_ref(file);

	return file;
}


/**
 * Add a reference to a file.
 *   @file: The file.
 */

void amp_file_ref(struct amp_file_t *file)
{
	file->refcnt++;
}

/**
 * Remove a reference from a file.
 *   @file: The file.
 */

void amp_file_unref(struct amp_file_t *file)
{
	if(--file->refcnt == 0)
		amp_cache_close(file->cache, file);
}


/**
 * Retrieve the buffer from a file.
 *   @file: The file.
 *   &returns: The buffer.
 */

struct dsp_buf_t *amp_file_buf(struct amp_file_t *file)
{
	return file->buf;
}
