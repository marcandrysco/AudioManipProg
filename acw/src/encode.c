#include "common.h"

/*
 * encoding definitions
 */
#define POOL_SIZE 4096

/**
 * Encode structure.
 *   @n: The number of nodes.
 *   @pool: Node pool.
 *   @free: The free node list.
 */
struct acw_encode_t {
	unsigned int n;
	struct node_t **node;

	struct pool_t *pool;
	struct node_t *free;
};

/**
 * Node structure.
 *   @init, addlo, addhi: Initial and added values.
 *   @bits, len, tot: The number of bits and the node and total lengths.
 *   @nbytes: The number of bytes.
 *   @refcnt: The reference count.
 *   @link: The linked node.
 */
struct node_t {
	int init, addlo, addhi;
	unsigned int bits, len;

	size_t nbytes;
	unsigned int refcnt;

	struct node_t *link;
};

/**
 * Node pool structure.
 *   @node: The node array.
 *   @next: The next pool.
 */
struct pool_t {
	struct pool_t *next;
	struct node_t node[POOL_SIZE];
};


/**
 * Allocate a node from the encoder.
 *   @enc: The encoder.
 *   &returns: The node.
 */
struct node_t *enc_alloc(struct acw_encode_t *enc)
{
	struct node_t *node;

	if(enc->free == NULL) {
		unsigned int i;
		struct pool_t *pool;

		pool = malloc(sizeof(struct pool_t));
		pool->next = enc->pool;
		enc->pool = pool;

		pool->node[0].link = NULL;
		for(i = 0; i < POOL_SIZE; i++)
			pool->node[i].link = &pool->node[i-1];

		enc->free = &pool->node[POOL_SIZE-1];
	}

	node = enc->free;
	enc->free = node->link;

	return node;
}

/**
 * Free a node from the encoder.
 *   @enc: The encoder.
 *   @node: The node.
 */
void enc_free(struct acw_encode_t *enc, struct node_t *node)
{
	node->link = enc->free;
	enc->free = node;
}


/**
 * Create a node.
 *   @enc: The encoder.
 *   @init: The initial value.
 *   @bits: The number of bits.
 *   @link: The linked node.
 */
struct node_t *node_new(struct acw_encode_t *enc, int init, unsigned int bits, struct node_t *link)
{
	struct node_t *node;

	node = enc_alloc(enc);
	node->addlo = ACW_ADD_MIN;
	node->addhi = ACW_ADD_MAX;
	node->bits = bits;
	node->link = link;

	switch(bits) {
	case 24:
		node->nbytes = 5;
		node->init = 0;
		node->len = 1;
		break;
	case 16:
		node->nbytes = 6;
		node->init = init;
		node->len = 0;
		break;
	case 8:
		node->nbytes = 6;
		node->init = init;
		node->len = 0;
		break;
	case 4:
		node->nbytes = 6;
		node->init = init;
		node->len = 0;
		break;

	default:
		fatal("Invalid bit width '%u'.", bits);
	}

	if(link != NULL) {
		link->refcnt++;
		node->nbytes += link->nbytes;
	}

	return node;
}

/**
 * Copy a node.
 *   @enc: The encoder.
 *   @node: The node.
 *   &returns: The copy.
 */
struct node_t *node_copy(struct acw_encode_t *enc, struct node_t *node)
{
	struct node_t *copy;

	copy = enc_alloc(enc);
	*copy = *node;

	if(copy->link)
		copy->link->refcnt++;

	return copy;
}

/**
 * Delete a node.
 *   @enc: The encoder.
 *   @node: The node.
 */
void node_delete(struct acw_encode_t *enc, struct node_t *node)
{
	struct node_t *tmp;

	while(node != NULL) {
		if(--node->refcnt > 0)
			break;

		tmp = node;
		node = tmp->link;

		enc_free(enc, tmp);
	}
}


/**
 * Create an encoder.
 *   @n: The number of nodes.
 *   &returns: The encoder.
 */
struct acw_encode_t *acw_encode_new(unsigned int n)
{
	unsigned int i;
	struct acw_encode_t *enc;

	enc = malloc(sizeof(struct acw_encode_t));
	enc->n = n;
	enc->pool = NULL;
	enc->free = NULL;
	enc->node = malloc(5 * n * sizeof(void *));

	for(i = 0; i < 5 * n; i++)
		enc->node[i] = NULL;

	return enc;
}

/**
 * Process a value on the encoder.
 *   @enc: The encoder.
 *   @val: The value.
 */
void acw_encode_proc(struct acw_encode_t *enc, int val)
{
	if(enc->node[0] == NULL) {
		enc->node[0] = node_new(enc, val, 24, NULL);
		enc->node[1] = node_new(enc, val, 16, NULL);
		enc->node[2] = node_new(enc, val, 8, NULL);
		enc->node[3] = node_new(enc, val, 4, NULL);
	}
	else {
		unsigned int i, avail = enc->n;
		struct node_t *tmp;

		for(i = 0; i < enc->n; i++) {
			if(enc->node[i] == NULL)
				continue;

			tmp = node_copy(enc, enc->node[i]);

			enc->node[avail++] = node_new(enc, val, 24, tmp);
			enc->node[avail++] = node_new(enc, val, 16, tmp);
			enc->node[avail++] = node_new(enc, val, 8, tmp);
			enc->node[avail++] = node_new(enc, val, 4, tmp);

			//if(!node_expand(node[i], arr[idx]))
				//node_delete(node[i]), node[i] = NULL;

			node_delete(enc, tmp);
		}
	}
}

void acw_encode_done(struct acw_encode_t *enc)
{
	struct pool_t *pool;

	while(enc->pool != NULL) {
		pool = enc->pool;
		enc->pool = pool->next;

		free(pool);
	}

	free(enc->node);
	free(enc);
}
