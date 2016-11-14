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
 *   @id: The identifier.
 *   @next: The next track.
 *   @data: The data tree root.
 */
struct web_track_t {
	char *id;
	struct web_track_t *next;

	struct avltree_root_t data;
};

/**
 * Track data structure.
 *   @idx: The index.
 *   @buf: The audio buffer.
 *   @node: The tree node.
 */
struct web_data_t {
#define WEB_DATA_LEN (8*1024)
	int idx;
	float buf[WEB_DATA_LEN];

	struct avltree_node_t node;
};


/*
 * local delcarations
 */
static struct web_track_t *track_new(char *id);
static void track_delete(struct web_track_t *track);


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
	struct web_track_t *track;

	while(rec->track != NULL) {
		track = rec->track;
		rec->track = track->next;

		track_delete(track);
	}

	free(rec);
}


/**
 * Add a track to the multitrack recorder.
 *   @rec: The recorder.
 *   @id: Consumed. The identifier.
 */
void web_mulrec_add(struct web_mulrec_t *rec, char *id)
{
	struct web_track_t **track;

	track = &rec->track;
	while(*track != NULL)
		track = &(*track)->next;

	*track = track_new(id);
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


/**
 * Retrieve the information from a multitrack recorder.
 *   @rec: The recorder.
 *   @file: The file.
 */
void web_mulrec_print(struct web_mulrec_t *rec, struct io_file_t file)
{
}


/**
 * Load a multitrack recorder from disk.
 *   @rec: The recorder.
 *   &returns: Error.
 */
char *web_mulrec_load(struct web_mulrec_t *rec)
{
	return NULL;
}

/**
 * Save a multitrack recorder to disk.
 *   @rec: The recorder.
 *   &returns: Error.
 */
char *web_mulrec_save(struct web_mulrec_t *rec)
{
	return NULL;
}


/**
 * Create a new track.
 *   @id: Consumed. The identifier.
 *   &returns: The track.
 */
static struct web_track_t *track_new(char *id)
{
	struct web_track_t *track;

	track = malloc(sizeof(struct web_track_t));
	track->id = id;
	track->next = NULL;
	track->data = avltree_root_init(compare_int);

	return track;
}

/**
 * Delete a track.
 *   @track: The track.
 */
static void track_delete(struct web_track_t *track)
{
	free(track->id);
	free(track);
}


/**
 * Retrieve the index of the track.
 *   @rec: The recorder.
 *   @track: The track.
 */
unsigned int track_idx(struct web_mulrec_t *rec, struct web_track_t *track)
{
	unsigned int idx = 0;
	struct web_track_t *iter;

	for(iter = rec->track; iter != track; iter = iter->next)
		idx++;

	return idx;
}
