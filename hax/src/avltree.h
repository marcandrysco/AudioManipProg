#ifndef AVLTREE_H
#define AVLTREE_H

/**
 * AVL tree root structure.
 *   @count: The number of nodes.
 *   @node: The root node.
 *   @compare: The comparison callback.
 */
struct avltree_root_t {
	unsigned int count;
	struct avltree_node_t *node;

	compare_f compare;
};

/**
 * AVL tree node storage.
 *   @bal: The balance factor.
 *   @ref: The reference.
 *   @parent, left, right: The parent, left, and right children.
 */
struct avltree_node_t {
	int_fast8_t bal;

	void *ref;
	struct avltree_root_t *root;
	struct avltree_node_t *parent, *left, *right;
};

/*
 * avl tree root function declarations
 */
struct avltree_root_t avltree_root_init(compare_f compare);
void avltree_root_destroy(struct avltree_root_t *root, ssize_t offset, delete_f delete);

struct avltree_node_t *avltree_root_first(struct avltree_root_t *root);
struct avltree_node_t *avltree_root_last(struct avltree_root_t *root);

struct avltree_node_t *avltree_root_lookup(struct avltree_root_t *root, const void *ref);
struct avltree_node_t *avltree_root_atleast(struct avltree_root_t *root, const void *ref);
struct avltree_node_t *avltree_root_atmost(struct avltree_root_t *root, const void *ref);

void avltree_root_insert(struct avltree_root_t *root, struct avltree_node_t *node);
struct avltree_node_t *avltree_root_remove(struct avltree_root_t *root, const void *ref);

#endif
