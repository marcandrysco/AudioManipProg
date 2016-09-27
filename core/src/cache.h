#ifndef CACHE_H
#define CACHE_H

/*
 * cache declarations
 */
struct amp_cache_t;

struct amp_cache_t *amp_cache_new(void);
void amp_cache_delete(struct amp_cache_t *cache);

struct amp_file_t *amp_cache_lookup(struct amp_cache_t *cache, const char *path, unsigned int chan);

struct amp_file_t *amp_cache_open(struct amp_cache_t *cache, const char *path, unsigned int chan, unsigned int rate);
void amp_cache_close(struct amp_cache_t *cache, struct amp_file_t *file);

/*
 *  file declarations
 */
struct amp_file_t;

struct amp_file_t *amp_file_copy(struct amp_file_t *file);
void amp_file_delete(struct amp_file_t *file);

void amp_file_ref(struct amp_file_t *file);
void amp_file_unref(struct amp_file_t *file);

struct dsp_buf_t *amp_file_buf(struct amp_file_t *file);

#endif
