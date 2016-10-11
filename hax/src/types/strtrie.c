#include "../common.h"


/*
 * local declarations
 */
static struct strtrie_inst_t *inst_new(char *str, void *value);
static void inst_delete(struct strtrie_inst_t *inst);

static struct strtrie_inst_t *root_add(struct strtrie_root_t *root, const char *str, void *value);


/**
 * Lookup an instance.
 *   @root: The root.
 *   @ch: The character.
 */
static struct strtrie_inst_t *inst_lookup(struct strtrie_root_t *root, char ch)
{
	struct avltree_node_t *node;

	node = avltree_root_lookup(&root->avl, &ch);
	return node ? getparent(node, struct strtrie_inst_t, node) : NULL;
}


/**
 * Initailize a string trie.
 *   &returns: The trie.
 */
struct strtrie_t strtrie_init(void)
{
	struct strtrie_t trie;

	trie.root.avl = avltree_root_init(compare_char);

	return trie;
}

/**
 * Destroy a string trie.
 *   @trie: The trie.
 */
void strtrie_destroy(struct strtrie_t *trie)
{
	avltree_root_destroy(&trie->root.avl, offsetof(struct strtrie_inst_t, node), (delete_f)inst_delete);
}


/**
 * Add a value to a string trie.
 *   @trie: The trie.
 *   @str: The string.
 *   @value: The value.
 *   &returns: The instance.
 */
struct strtrie_inst_t *strtrie_add(struct strtrie_t *trie, const char *str, const void *value)
{
	return root_add(&trie->root, str, (void *)value);
}

/**
 * Lookup a value given a string.
 *   @trie: The trie.
 *   @str: The string.
 *   &returns: The value.
 */
void *strtrie_lookup(struct strtrie_t *trie, const char *str)
{
	struct strtrie_root_t *root;
	struct strtrie_inst_t *inst;

	root = &trie->root;
	while(true) {
		inst = inst_lookup(root, *str);
		if(inst == NULL)
			return NULL;

		const char *ref = inst->str;
		while(*str != '\0') {
			if(*str != *ref)
				break;

			str++, ref++;
		}

		if((*str == '\0') && (*ref == '\0'))
			return inst->value;
		else if(*str == '\0')
			return NULL;
		else if(*ref == '\0')
			root = &inst->root;
		else
			return NULL;
	}
}

/**
 * Retrieve the first instance from a root.
 *   @root: The root.
 *   &returns: The instance or null.
 */
struct strtrie_inst_t *strtrie_first(struct strtrie_root_t *root)
{
	struct avltree_node_t *node;

	node = avltree_root_first(&root->avl);
	return node ? getparent(node, struct strtrie_inst_t, node) : NULL;
}

/**
 * Retrieve the last instance from a root.
 *   @root: The root.
 *   &returns: The instance or null.
 */
struct strtrie_inst_t *strtrie_last(struct strtrie_root_t *root)
{
	struct avltree_node_t *node;

	node = avltree_root_last(&root->avl);
	return node ? getparent(node, struct strtrie_inst_t, node) : NULL;
}

/**
 * Retrieve the previous node given an instance.
 *   @inst: The instance.
 *   &returns: The previous instance or null.
 */
struct strtrie_inst_t *strtrie_prev(struct strtrie_inst_t *inst)
{
	struct avltree_node_t *node;

	node = avltree_node_prev(&inst->node);
	return node ? getparent(node, struct strtrie_inst_t, node) : NULL;
}

/**
 * Retrieve the next node given an instance.
 *   @inst: The instance.
 *   &returns: The next instance or null.
 */
struct strtrie_inst_t *strtrie_next(struct strtrie_inst_t *inst)
{
	struct avltree_node_t *node;

	node = avltree_node_next(&inst->node);
	return node ? getparent(node, struct strtrie_inst_t, node) : NULL;
}


/**
 * Create a new instance.
 *   @str: Consumed. The string.
 *   @value: The value.
 *   &returns: The instance.
 */
static struct strtrie_inst_t *inst_new(char *str, void *value)
{
	struct strtrie_inst_t *inst;

	inst = malloc(sizeof(struct strtrie_inst_t));
	inst->str = str;
	inst->value = value;
	inst->root.avl = avltree_root_init(compare_char);
	inst->node.ref = inst->str;

	return inst;
}

/**
 * Delete an instance.
 *   @inst: The instance.
 */
static void inst_delete(struct strtrie_inst_t *inst)
{
	avltree_root_destroy(&inst->root.avl, offsetof(struct strtrie_inst_t, node), (delete_f)inst_delete);
	free(inst->str);
	free(inst);
}


/**
 * Add a string-value pair to a root.
 *   @roo: The root.
 *   @str: The string.
 *   @value: The value.
 *   &returns: The added instance.
 */
static struct strtrie_inst_t *root_add(struct strtrie_root_t *root, const char *str, void *value)
{
	assert((value != NULL) && (*str != '\0'));

	struct strtrie_inst_t *inst;
	inst = inst_lookup(root, *str);
	if(inst == NULL) {
		inst = inst_new(strdup(str), value);
		avltree_root_insert(&root->avl, &inst->node);

		return inst;
	}
	else {
		char *ref = inst->str;
		while(*str != '\0') {
			if(*str != *ref)
				break;

			str++, ref++;
		}

		if((*str == '\0') && (*ref == '\0')) {
			if(inst->value != NULL)
				fatal("Value already exists.");

			inst->value = value;

			return inst;
		}
		else if(*str == '\0') {
			struct strtrie_inst_t *left;

			left = inst_new(strdup(ref), inst->value);
			left->root = inst->root;

			*ref = '\0';
			inst->str = realloc(inst->str, ref - inst->str + 1);
			inst->node.ref = inst->str;
			inst->value = value;
			inst->root.avl = avltree_root_init(compare_char);
			avltree_root_insert(&inst->root.avl, &left->node);

			return left;
		}
		else if(*ref == '\0')
			return root_add(&inst->root, str, value);
		else {
			struct strtrie_inst_t *left, *right;

			right = inst_new(strdup(str), value);
			left = inst_new(strdup(ref), inst->value);
			left->root = inst->root;

			*ref = '\0';
			inst->str = realloc(inst->str, ref - inst->str + 1);
			inst->node.ref = inst->str;
			inst->value = NULL;
			inst->root.avl = avltree_root_init(compare_char);
			avltree_root_insert(&inst->root.avl, &left->node);
			avltree_root_insert(&inst->root.avl, &right->node);

			return right;
		}
	}
}
