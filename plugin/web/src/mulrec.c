#include "common.h"


/**
 * Multitrack recorder structure.
 *   @id: The identifier.
 *   @track: The track list.
 */
struct web_mulrec_t {
	const char *id;
	struct web_track_t *track;
};

/**
 * Track structure.
 *   @next: The next track.
 */
struct web_track_t {
	struct web_track_t *next;
};


/**
 * Create a new multitrack recorder.
 *   @id: The identifier.
 *   &returns: The recorder.
 */
struct web_mulrec_t *web_mulrec_new(const char *id)
{
	struct web_mulrec_t *rec;

	rec = malloc(sizeof(struct web_mulrec_t));
	rec->id = id;
	rec->track = NULL;

	return rec;
}

/**
 * Delete a multitrack recorder.
 *   @rec: The recorder.
 */
void web_mulrec_delete(struct web_mulrec_t *rec)
{
	free(rec);
}


/**
 * Process information on a multitrack recorder.
 *   @mach: The recorder.
 *   @info: The information.
 */
void web_mulrec_info(struct web_mulrec_t *rec, struct amp_info_t info)
{
}

/**
 * Process data on a multitrack recorder.
 *   @mach: The recorder.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool web_mulrec_proc(struct web_mulrec_t *rec, double **buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	return false;
}
