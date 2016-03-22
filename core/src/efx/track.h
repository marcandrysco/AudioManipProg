#ifndef MOD_TRACK_H
#define MOD_TRACK_H

/*
 * track definitions
 */
#define AMP_TRACK_LEN 4096

/**
 * Track structure.
 *   @path: The path.
 *   @tree: The instance tree.
 */
struct amp_track_t {
	char *path;
	struct avltree_root_t tree;
};

/**
 * Track data segment structure structure.
 *   @idx: The index.
 *   @node: The node.
 *   @buf: The buffer.
 */
struct amp_track_data_t {
	int idx;
	struct avltree_node_t node;

	float buf[AMP_TRACK_LEN];
};


/*
 * track declarations
 */
extern const struct amp_effect_i amp_track_iface;

struct amp_track_t *amp_track_new(char *path);
struct amp_track_t *amp_track_copy(struct amp_track_t *track);
void amp_track_delete(struct amp_track_t *track);

void amp_track_read(struct amp_track_t *track, double *buf, int idx, unsigned int len);
void amp_track_write(struct amp_track_t *track, const double *buf, int idx, unsigned int len);

void amp_track_info(struct amp_track_t *track, struct amp_info_t info);
bool amp_track_proc(struct amp_track_t *track, double *buf, struct amp_time_t *time, unsigned int len);

#endif
