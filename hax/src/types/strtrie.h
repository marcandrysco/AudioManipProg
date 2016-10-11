#ifndef TYPES_STRTRIE_H
#define TYPES_STRTRIE_H

/**
 * String trie root.
 *   @avl: AVL tree root.
 */
struct strtrie_root_t {
	struct avltree_root_t avl;
};

/**
 * String trie node structre.
 *   @str: The string.
 *   @value: The value.
 *   @root: The root.
 *   @node: The node.
 */
struct strtrie_inst_t {
	char *str;
	void *value;

	struct strtrie_root_t root;
	struct avltree_node_t node;
};

/**
 * String trie structure.
 *   @root: The tree root.
 */
struct strtrie_t {
	struct strtrie_root_t root;
};

/*
 * string trie declarations
 */
struct strtrie_t strtrie_init(void);
void strtrie_destroy(struct strtrie_t *trie);

void *strtrie_lookup(struct strtrie_t *trie, const char *str);
struct strtrie_inst_t *strtrie_add(struct strtrie_t *trie, const char *str, const void *value);

struct strtrie_inst_t *strtrie_first(struct strtrie_root_t *root);
struct strtrie_inst_t *strtrie_last(struct strtrie_root_t *root);
struct strtrie_inst_t *strtrie_prev(struct strtrie_inst_t *inst);
struct strtrie_inst_t *strtrie_next(struct strtrie_inst_t *inst);

#endif
