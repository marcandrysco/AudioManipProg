#include "../common.h"


/*
 * local declarations
 */
static struct amp_track_data_t *data_lookup(struct amp_track_t *track, int idx);
static struct amp_track_data_t *data_create(struct amp_track_t *track, int idx);


/*
 * global variables
 */
const struct amp_effect_i amp_track_iface = {
	(amp_info_f)amp_track_info,
	(amp_effect_f)amp_track_proc,
	(amp_copy_f)amp_track_copy,
	(amp_delete_f)amp_track_delete
};


/**
 * Create a new track.
 *   @path: The path.
 *   &returns: The track.
 */
struct amp_track_t *amp_track_new(char *path)
{
	struct amp_track_t *track;

	track = malloc(sizeof(struct amp_track_t));
	track->path = path;
	track->tree = avltree_root_init(compare_int);

	return track;
}

/**
 * Copy a new track.
 *   @track: The original track.
 *   &returns: The copied track.
 */
struct amp_track_t *amp_track_copy(struct amp_track_t *track)
{
	struct amp_track_t *copy;

	copy = malloc(sizeof(struct amp_track_t));
	copy->path = strdup(track->path);
	copy->tree = avltree_root_init(compare_int);

	return copy;
}

/**
 * Delete a track.
 *   @track: The track.
 */
void amp_track_delete(struct amp_track_t *track)
{
	avltree_root_destroy(&track->tree, offsetof(struct amp_track_data_t, node), free);
	free(track->path);
	free(track);
}


/**
 * Read data from a track.
 *   @track: The track.
 *   @buf: The buffer.
 *   @idx: The starting index.
 *   @len: The buffer length.
 */
void amp_track_read(struct amp_track_t *track, double *buf, int idx, unsigned int len)
{
	unsigned int sel;
	struct amp_track_data_t *data;

	data = data_lookup(track, idx / AMP_TRACK_LEN);

	while(len-- > 0) {
		sel = idx % AMP_TRACK_LEN;
		if(sel == 0)
			data = data_lookup(track, idx / AMP_TRACK_LEN);

		*(buf++) = data ? data->buf[sel] : 0.0;
		idx++;
	}
}

/**
 * Write data to a track instance.
 *   @inst: The instance.
 *   @buf: The buffer.
 *   @idx: The starting index.
 *   @len: The buffer length.
 */
void amp_track_write(struct amp_track_t *track, const double *buf, int idx, unsigned int len)
{
	unsigned int sel;
	struct amp_track_data_t *data;

	data = data_create(track, idx / AMP_TRACK_LEN);

	while(len-- > 0) {
		sel = idx % AMP_TRACK_LEN;
		if(sel == 0)
			data = data_create(track, idx / AMP_TRACK_LEN);

		data->buf[sel] = *(buf++);
		idx++;
	}
}


/**
 * Lookup the track data segment associated with an index.
 *   @track: The track.
 *   @idx: The index.
 *   &returns: The data segment or null.
 */
static struct amp_track_data_t *data_lookup(struct amp_track_t *track, int idx)
{
	struct avltree_node_t *node;

	node = avltree_root_lookup(&track->tree, &idx);
	return node ? getparent(node, struct amp_track_data_t, node) : NULL;
}

/**
 * Create or retrieve the track data segment associated with an index.
 *   @track: The track.
 *   @idx: The index.
 *   &returns: The data segment.
 */
static struct amp_track_data_t *data_create(struct amp_track_t *track, int idx)
{
	struct avltree_node_t *node;
	struct amp_track_data_t *data;

	node = avltree_root_lookup(&track->tree, &idx);
	if(node == NULL) {
		data = malloc(sizeof(struct amp_track_data_t));
		data->idx = idx;
		data->node.ref = &data-idx;
		dsp_zero_f(data->buf, AMP_TRACK_LEN);

		avltree_root_insert(&track->tree, &data->node);
	}
	else
		data = getparent(node, struct amp_track_data_t, node);

	return data;
}


/**
 * Handle information on a track.
 *   @track: The track.
 *   @info: The information.
 */
void amp_track_info(struct amp_track_t *track, struct amp_info_t info)
{
}

/**
 * Process a track.
 *   @track: The track.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_track_proc(struct amp_track_t *track, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	return false;
}
