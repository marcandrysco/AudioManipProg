#include "../common.h"

#define AMP_LOOP_CNT 16

/**
 * Loop structure.
 *   @path: The file path.
 *   @buf: The buffer.
 *   @on: The writing flag.
 *   @off: The offset and modulus times.
 *   @play: The record identifier.
 *   @sel: The selected read and sample rate.
 *   @wr, rd: The read and write indices.
 */
struct amp_loop_t {
	char *path;
	struct dsp_buf_t *buf;

	bool on;
	struct amp_time_t off, mod;
	struct amp_id_t rec;

	unsigned int sel, rate;
	int wr, rd[AMP_LOOP_CNT];
};


/*
 * global variables
 */
const struct amp_effect_i amp_loop_iface = {
	(amp_info_f)amp_loop_info,
	(amp_effect_f)amp_loop_proc,
	(amp_copy_f)amp_loop_copy,
	(amp_delete_f)amp_loop_delete
};


/**
 * Create a loop effect.
 *   @path: The buffer path.
 *   @mod: The repeat modulus.
 *   @len: The maximum loop length.
 *   @rec: The record identifier.
 *   &returns: The loop.
 */
struct amp_loop_t *amp_loop_new(char *path, struct amp_time_t mod, unsigned int len, struct amp_id_t rec)
{
	unsigned int i;
	struct amp_loop_t *loop;

	loop = malloc(sizeof(struct amp_loop_t));
	loop->path = path;
	loop->buf = dsp_buf_new(len);
	loop->off = (struct amp_time_t){ INT_MIN, INT_MIN, 0.0 };
	loop->mod = mod;
	loop->rec = rec;
	loop->on = false;
	loop->wr = 0;
	loop->sel = 0;

	for(i = 0; i < AMP_LOOP_CNT; i++)
		loop->rd[i] = INT_MAX;

	return loop;
}

/**
 * Copy a loop effect.
 *   @loop: The original loop.
 *   &returns: The copied loop.
 */
struct amp_loop_t *amp_loop_copy(struct amp_loop_t *loop)
{
	return amp_loop_new(strdup(loop->path), loop->mod, loop->buf->len, loop->rec);
}

/**
 * Delete a loop effect.
 *   @loop: The loop.
 */
void amp_loop_delete(struct amp_loop_t *loop)
{
	dsp_buf_delete(loop->buf);
	free(loop->path);
	free(loop);
}


/**
 * Create a loop from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_loop_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit
	char *path;
	double len, tm;
	struct amp_time_t mod;
	int dev, key;
	struct amp_id_t rec;

	chkfail(amp_match_unpack(value, "(s,(d,f,f),f,(d,d))", &path, &mod.bar, &mod.beat, &tm, &len, &dev, &key));

	mod.idx = tm * amp_core_rate(env);
	rec.dev = dev;
	rec.key = key;

	*ret = amp_pack_effect((struct amp_effect_t){ amp_loop_new(path, mod, len * amp_core_rate(env), rec), &amp_loop_iface });
	return NULL;
#undef onexit
}


/**
 * Handle information on a loop.
 *   @loop: The loop.
 *   @info: The information.
 */
void amp_loop_info(struct amp_loop_t *loop, struct amp_info_t info)
{
}


/**
 * Process a loop.
 *   @loop: The loop.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool amp_loop_proc(struct amp_loop_t *loop, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{
	struct amp_time_t left, right;
	unsigned int i, j, n = 0;

	left = amp_time_mod(time[0], loop->mod);
	for(i = 0; i < len; i++) {
		struct amp_event_t *event;

		while((event = amp_queue_event(queue, &n, i)) != NULL) {
			if((event->dev == loop->rec.dev) && (event->key == loop->rec.key))
				break;
		}

		right = amp_time_mod(time[i+1], loop->mod);

		if(event != NULL) {
			if(event->val > 0) {
				printf("REC!\n");
				loop->on = true;
				loop->wr = 0;
				loop->off = time[i];
				loop->sel = 0;

				for(j = 0; j < AMP_LOOP_CNT; j++)
					loop->rd[j] = INT_MAX;
			}
			else
				printf("off!\n"),
				loop->on = false;
		}
		else if(!amp_time_isequal(left, right)) {
			if(amp_time_between(loop->off, left, right) && (loop->wr > 0)) {
				loop->sel = (loop->sel - 1 + AMP_LOOP_CNT) % AMP_LOOP_CNT;
				loop->rd[loop->sel] = 0;
			}
		}

		if(loop->on) {
			if(loop->wr < loop->buf->len)
				loop->buf->arr[loop->wr++] = buf[i];
			else
				loop->on = false;
		}

		for(j = 0; j < AMP_LOOP_CNT; j++) {
			unsigned int idx = (j + loop->sel) % AMP_LOOP_CNT;

			if(loop->rd[idx] >= loop->wr)
				break;

			buf[i] += loop->buf->arr[loop->rd[idx]++];
		}

		left = right;
	}
	/*
	unsigned int i, j, n = 0;

	for(i = 0; i < len; i++) {
		double v = 0;
		struct amp_event_t *event;

		while((event = amp_queue_event(queue, &n, i)) != NULL) {
			if((event->dev == loop->rec.dev) && (event->key == loop->rec.key))
				loop->wr = (event->val > 0) ? 0 : INT_MAX;

			printf("loop->wr: %d\n", !loop->wr);

			if((event->dev == loop->play.dev) && (event->key == loop->play.key)) {
				unsigned int i;

				if(event->val > 0) {
					for(i = 0; i < loop->n; i++) {
						if(loop->rd[i] == INT_MAX)
							break;
					}

					if(i < loop->n)
						loop->rd[i] = 0;
				}
				else {
					for(i = 0; i < loop->n; i++)
						loop->rd[i] = INT_MAX;
				}
			}
		}

		for(j = 0; j < loop->n; j++) {
			if(loop->rd[j] == INT_MAX)
				continue;

			if(loop->rd[j] < loop->max)
				v += loop->buf->arr[loop->rd[j]++];
			else
				loop->rd[j] = INT_MAX;
		}

		if(loop->wr != INT_MAX) {
			if(loop->wr < loop->buf->len) {
				loop->buf->arr[loop->wr] = buf[i];
				if(loop->wr > loop->max)
					loop->max = loop->wr;

				loop->wr++;
			}
			else
				loop->wr = INT_MAX;
		}

		buf[i] = v;
	}
	*/

	return false;
}
