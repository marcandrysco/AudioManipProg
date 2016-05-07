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

	const void *ref;
	struct avltree_root_t *root;
	struct avltree_node_t *parent, *left, *right;
};


/**
 * Pointer-based AVL tree.
 *   @root: The root.
 *   @delete: Deletion callback.
 */
struct avltree_t {
	struct avltree_root_t root;
	delete_f delete;
};

/**
 * Instance structure.
 *   @val: The value.
 *   @tree: The parent tree.
 *   @node: The node.
 */
struct avltree_inst_t {
	void *val;

	struct avltree_t *tree;
	struct avltree_node_t node;
};


/*
 * avl tree root function declarations
 */
struct avltree_root_t avltree_root_init(compare_f compare);
void avltree_root_destroy(struct avltree_root_t *root, ssize_t offset, delete_f delete);

struct avltree_node_t *avltree_root_first(struct avltree_root_t *root);
struct avltree_node_t *avltree_root_last(struct avltree_root_t *root);
struct avltree_node_t *avltree_node_prev(struct avltree_node_t *node);
struct avltree_node_t *avltree_node_next(struct avltree_node_t *node);

struct avltree_node_t *avltree_root_lookup(struct avltree_root_t *root, const void *ref);
struct avltree_node_t *avltree_root_atleast(struct avltree_root_t *root, const void *ref);
struct avltree_node_t *avltree_root_atmost(struct avltree_root_t *root, const void *ref);

void avltree_root_insert(struct avltree_root_t *root, struct avltree_node_t *node);
struct avltree_node_t *avltree_root_remove(struct avltree_root_t *root, const void *ref);

/*
 * avl tree declarations
 */
struct avltree_t avltree_init(compare_f compare, delete_f delete);
void avltree_destroy(struct avltree_t *tree);

void *avltree_lookup(struct avltree_t *tree, const void *key);
struct avltree_inst_t *avltree_insert(struct avltree_t *tree, const void *key, void *val);

struct avltree_inst_t *avltree_first(struct avltree_t *tree);
struct avltree_inst_t *avltree_prev(struct avltree_inst_t *inst);
struct avltree_inst_t *avltree_last(struct avltree_t *tree);
struct avltree_inst_t *avltree_next(struct avltree_inst_t *inst);

#endif
