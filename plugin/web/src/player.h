#ifndef PLAYER_H
#define PLAYER_H

/**
 * Location structure.
 *   @bar: The bar.
 *   @beat: The beat.
 */
struct amp_loc_t {
	int bar;
	double beat;
};

static inline struct amp_loc_t amp_loc(int bar, double beat) {
	return (struct amp_loc_t){ bar, beat };
}

/**
 * Compare two loations.
 *   @left: The left location.
 *   @right: The right location.
 *   &returns: Their order.
 */
static inline int amp_loc_cmp(struct amp_loc_t left, struct amp_loc_t right)
{
	if(left.bar > right.bar)
		return 2;
	else if(left.bar < right.bar)
		return -2;
	else if(left.beat > right.beat)
		return 1;
	else if(left.beat < right.beat)
		return -1;
	else
		return 0;
}

/**
 * Check if two locations are near eachother, close enough to be considered
 * the "same".
 *   @left; The left location.
 *   @right: The right location.
 *   &returns: True if near.
 */
static inline bool amp_loc_near(struct amp_loc_t left, struct amp_loc_t right)
{
	if(left.bar != right.bar)
		return false;

	return fabs(left.beat - right.beat) < 1e-5;
}

static inline struct amp_loc_t amp_loc_idx(unsigned int idx, unsigned int ndivs, double nbeats)
{
	struct amp_loc_t loc;

	loc.bar = idx / (ndivs * nbeats);
	loc.beat = (double)(idx - (ndivs * nbeats) * loc.bar) / (double)ndivs;

	return loc;
}


/*
 * player declarations
 */
struct web_player_t;

struct web_player_t *web_player_new(const char *id);
char *web_player_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
void web_player_delete(struct web_player_t *player);

struct web_player_inst_t *web_player_lookup(struct web_player_t *player, struct amp_loc_t begin, struct amp_loc_t end, uint16_t key);
void web_player_add(struct web_player_t *player, struct amp_loc_t begin, struct amp_loc_t end, uint16_t key, uint16_t vel);
void web_player_remove(struct web_player_t *player, struct amp_loc_t begin, struct amp_loc_t end, uint16_t key);

void web_player_info(struct web_player_t *player, struct amp_info_t info);
bool web_player_proc(struct web_player_t *player, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue);

void web_player_print(struct web_player_t *player, struct io_file_t file);
struct io_chunk_t web_player_chunk(struct web_player_t *player);

bool web_player_req(struct web_player_t *player, const char *path, struct http_args_t *args);

void web_player_load(struct web_player_t *player, const char *path);
void web_player_save(struct web_player_t *player, const char *path);

/*
 * player configuration declarations
 */
struct web_player_conf_t;

void web_player_conf_init(struct web_player_conf_t *conf);
void web_player_conf_destroy(struct web_player_conf_t *conf);

#endif
