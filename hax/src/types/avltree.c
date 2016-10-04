#include "../common.h"

/*
 * local declarations
 */
static void node_destroy(struct avltree_node_t *node, ssize_t offset, delete_f delete);

static struct avltree_node_t *node_first(struct avltree_node_t *node);
static struct avltree_node_t *node_last(struct avltree_node_t *node);

static struct avltree_node_t *rotate_right(struct avltree_node_t *node);
static struct avltree_node_t *rotate_left(struct avltree_node_t *node);

static int_fast8_t getdir(struct avltree_node_t *node);
static struct avltree_node_t **getref(struct avltree_node_t *node);

static void inst_delete(struct avltree_inst_t *inst);


/**
 * Create an empty AVL tree root.
 *   @compare: The comparison.
 *   &returns: The empty root.
 */
struct avltree_root_t avltree_root_init(compare_f compare)
{
	return (struct avltree_root_t){ 0, NULL, compare };
}

/**
 * Destroy an AVL tree root.
 *   @root: The root.
 *   @offset; The offset.
 *   @delete: Deletion callack.
 */
void avltree_root_destroy(struct avltree_root_t *root, ssize_t offset, delete_f delete)
{
	node_destroy(root->node, offset, delete);
}
static void node_destroy(struct avltree_node_t *node, ssize_t offset, delete_f delete)
{
	if(node == NULL)
		return;

	node_destroy(node->left, offset, delete);
	node_destroy(node->right, offset, delete);
	delete((void *)node - offset);
}


/**
 * Retrieve the first node from an AVL tree root.
 *   @root: The root.
 *   &returns: The first node or null.
 */
struct avltree_node_t *avltree_root_first(struct avltree_root_t *root)
{
	return root->node ? node_first(root->node) : NULL;
}
static struct avltree_node_t *node_first(struct avltree_node_t *node)
{
	while(node->left != NULL)
		node = node->left;

	return node;
}

/**
 * Retrieve the last node from an AVL tree root.
 *   @root: The root.
 *   &returns: The last node or null.
 */
struct avltree_node_t *avltree_root_last(struct avltree_root_t *root)
{
	return root->node ? node_last(root->node) : NULL;
}
static struct avltree_node_t *node_last(struct avltree_node_t *node)
{
	while(node->right != NULL)
		node = node->right;

	return node;
}

/**
 * Retrieve the previous node.
 *   @node: The current node.
 *   &returns: The previous node or null.
 */
struct avltree_node_t *avltree_node_prev(struct avltree_node_t *node)
{
	if(node->left != NULL) {
		node = node->left;

		while(node->right != NULL)
			node = node->right;

		return node;
	}
	else {
		while(node->parent != NULL) {
			if(node->parent->right == node)
				break;

			node = node->parent;
		}

		return node->parent;
	}
}

/**
 * Retrieve the next node.
 *   @node: The current node.
 *   &returns: The next node or null.
 */
struct avltree_node_t *avltree_node_next(struct avltree_node_t *node)
{
	if(node->right != NULL) {
		node = node->right;

		while(node->left != NULL)
			node = node->left;

		return node;
	}
	else {
		while(node->parent != NULL) {
			if(node->parent->left == node)
				break;

			node = node->parent;
		}

		return node->parent;
	}
}


/**
 * Lookup a node by reference.
 *   @root; The root.
 *   @ref: The reference.
 *   &returns: The node or null.
 */
struct avltree_node_t *avltree_root_lookup(struct avltree_root_t *root, const void *ref)
{
	int cmp;
	struct avltree_node_t *node;

	node = root->node;
	while(node != NULL) {
		cmp = root->compare(node->ref, ref);
		if(cmp < 0)
			node = node->right;
		else if(cmp > 0)
			node = node->left;
		else
			return node;
	}

	return NULL;
}

/**
 * Look up an AVL tree node from the root of at least a given value.
 *   @root: The root.
 *   @key: The sought reference.
 *   &returns: The node if found, null otherwise.
 */
struct avltree_node_t *avltree_root_atleast(struct avltree_root_t *root, const void *ref)
{
	int cmp = 0;
	struct avltree_node_t *node = root->node, *prev = NULL;

	while(node != NULL) {
		prev = node;

		cmp = root->compare(node->ref, ref);
		if(cmp < 0)
			node = node->right;
		else if(cmp > 0)
			node = node->left;
		else
			return node;
	}

	if(cmp < 0)
		return avltree_node_next(prev);
	else
		return prev;
}

/**
 * Look up an AVL tree node from the root of at most a given value.
 *   @root: The root.
 *   @key: The sought reference.
 *   &returns: The node if found, null otherwise.
 */
struct avltree_node_t *avltree_root_atmost(struct avltree_root_t *root, const void *ref)
{
	int cmp = 0;
	struct avltree_node_t *node = root->node, *prev = NULL;

	while(node != NULL) {
		prev = node;

		cmp = root->compare(node->ref, ref);
		if(cmp < 0)
			node = node->right;
		else if(cmp > 0)
			node = node->left;
		else
			return node;
	}

	if(cmp > 0)
		return avltree_node_prev(prev);
	else
		return prev;
}


/**
 * Insert a node onto the root.
 *   @root; The root.
 *   @node: The node.
 */
void avltree_root_insert(struct avltree_root_t *root, struct avltree_node_t *ins)
{
	struct avltree_node_t **cur, *node, *parent = NULL;

	cur = &root->node;

	while(*cur != NULL) {
		parent = *cur;
		if(root->compare((*cur)->ref, ins->ref) <= 0)
			cur = &(*cur)->right;
		else
			cur = &(*cur)->left;
	}

	*cur = ins;
	ins->bal = 0;
	ins->root = root;
	ins->parent = parent;
	ins->left = ins->right = NULL;
	node = ins;
	root->count++;

	while((parent = node->parent) != NULL) {
		parent->bal += getdir(node);
		node = node->parent;
		if(node->bal > 1) {
			if(node->right->bal == -1)
				node->right = rotate_right(node->right);

			node = *getref(node) = rotate_left(node);
			break;
		}
		else if(node->bal < -1) {
			if(node->left->bal == 1)
				node->left = rotate_left(node->left);

			node = *getref(node) = rotate_right(node);
			break;
		}
		else if(node->bal == 0)
			break;
	}
}

/**
 * Remove a node from the root.
 *   @root; The root.
 *   @ref: The reference.
 *   &returns: The node.
 *   @node: The removed node if the reference is found.
 */
struct avltree_node_t *avltree_root_remove(struct avltree_root_t *root, const void *ref)
{
	int_fast8_t bal;
	struct avltree_node_t *rem, *node;

	rem = avltree_root_lookup(root, ref);
	if(rem == NULL)
		return NULL;

	if(rem->left != NULL)
		node = node_last(rem->left), bal = getdir(node);
	else if(rem->right != NULL)
		node = node_first(rem->right), bal = getdir(node);
	else
		node = NULL, bal = rem->parent ? getdir(rem) : 0;

	root->count--;
	*getref(rem) = node;

	if(node != NULL) {
		struct avltree_node_t *parent;

		if(node->left) {
			node->left->parent = node->parent;
			*getref(node) = node->left;
		}
		else if(node->right) {
			node->left->parent = node->parent;
			*getref(node) = node->left;
		}
		else {
			*getref(node) = NULL;
		}
		parent = (node->parent == rem) ? node : node->parent;

		node->bal = rem->bal;
		node->parent = rem->parent;
		node->left = rem->left;
		node->right = rem->right;

		if(node->left != NULL)
			node->left->parent = node;

		if(node->right != NULL)
			node->right->parent = node;

		node = parent;
	}
	else
		node = rem->parent;

	if(node != NULL) {
		while(true) {
			node->bal -= bal;

			if(node->bal > 1) {
				if(node->right->bal == -1)
					node->right = rotate_right(node->right);

				node = *getref(node) = rotate_left(node);

			}
			else if(node->bal < -1) {
				if(node->left->bal == 1)
					node->left = rotate_left(node->left);

				node = *getref(node) = rotate_right(node);

			}

			if(node->bal != 0)
				break;

			if(node->parent == NULL)
				break;

			bal = getdir(node);
			node = node->parent;
		}
	}

	return rem;
}

/**
 * Rotate right on a given node.
 *   @node: The base node.
 *   &returns: The new base node.
 */
static struct avltree_node_t *rotate_right(struct avltree_node_t *node)
{
	struct avltree_node_t *tmp;

	tmp = node->left;
	node->left = tmp->right;
	tmp->right = node;

	node->bal++;
	if(tmp->bal < 0)
		node->bal -= tmp->bal;

	tmp->bal++;
	if(node->bal > 0)
		tmp->bal += node->bal;

	tmp->parent = node->parent;
	node->parent = tmp;

	if(node->left != NULL)
		node->left->parent = node;

	return tmp;
}

/**
 * Rotate left on a given node.
 *   @node; The base node.
 *   &returns: The new base node.
 */
static struct avltree_node_t *rotate_left(struct avltree_node_t *node)
{
	struct avltree_node_t *tmp;

	tmp = node->right;
	node->right = tmp->left;
	tmp->left = node;

	node->bal--;
	if(tmp->bal > 0)
		node->bal -= tmp->bal;

	tmp->bal--;
	if(node->bal < 0)
		tmp->bal += node->bal;

	tmp->parent = node->parent;
	node->parent = tmp;

	if(node->right != NULL)
		node->right->parent = node;

	return tmp;
}


/**
 * Get a reference to the node from either the parent or root.
 *   @node: The node.
 *   &returns: The reference.
 */
static struct avltree_node_t **getref(struct avltree_node_t *node)
{
	if(node->parent == NULL)
		return &node->root->node;
	else if(node->parent->left == node)
		return &node->parent->left;
	else
		return &node->parent->right;
}

/**
 * Retrieve the direction of a node relative to its parent.
 *   @node: The node.
 *   &returns: The direction as '1' or '-1'.
 */
static int_fast8_t getdir(struct avltree_node_t *node)
{
	return (node->parent->right == node) ? 1 : -1;
}



/**
 * Initialize an AVL tree.
 *   @compare: The comparison function.
 *   @delete: the deletion function.
 *   &returns: The AVL Tree.
 */
struct avltree_t avltree_init(compare_f compare, delete_f delete)
{
	return (struct avltree_t){ avltree_root_init(compare), delete };
}

/**
 * Destroy an AVL tree.
 *   @tree: The tree.
 */
void avltree_destroy(struct avltree_t *tree)
{
	avltree_root_destroy(&tree->root, offsetof(struct avltree_inst_t, node), (delete_f)inst_delete);
}
static void inst_delete(struct avltree_inst_t *inst)
{
	inst->tree->delete(inst->val);
	free(inst);
}


/**
 * Lookup a value in the tree.
 *   @tree: The tree.
 *   @key: The key.
 *   &returns: The value or null.
 */
void *avltree_lookup(struct avltree_t *tree, const void *key)
{
	struct avltree_node_t *node;

	node = avltree_root_lookup(&tree->root, key);
	return node ? getparent(node, struct avltree_inst_t, node)->val : NULL;
}

/**
 * Insert a key-value pair into the AVL tree.
 *   @tree: The AVL tree.
 *   @key: The key.
 *   @val: The value.
 *   &returns: The inserted instance.
 */
struct avltree_inst_t *avltree_insert(struct avltree_t *tree, const void *key, void *val)
{
	struct avltree_inst_t *inst;

	inst = malloc(sizeof(struct avltree_inst_t));
	inst->val = val;
	inst->tree = tree;
	inst->node.ref = key;
	avltree_root_insert(&tree->root, &inst->node);

	return inst;
}

/**
 * Remove a key-value pair from the AVL tree.
 *   @tree: The tree.
 *   @key: The key.
 *   &returns: The removed value.
 */
void *avltree_remove(struct avltree_t *tree, const void *key)
{
	void *val;
	struct avltree_inst_t *inst;
	struct avltree_node_t *node;

	node = avltree_root_remove(&tree->root, key);
	if(node == NULL)
		return NULL;

	inst = getparent(node, struct avltree_inst_t, node);
	val = inst->val;
	free(inst);

	return val;
}


/**
 * Retrieve the first instance from the tree.
 *   @tree: The tree.
 *   &returns: The instance or null.
 */
struct avltree_inst_t *avltree_first(struct avltree_t *tree)
{
	struct avltree_node_t *node;

	node = avltree_root_first(&tree->root);
	return node ? getparent(node, struct avltree_inst_t, node): NULL;
}

/**
 * Retrieve the last instance from the tree.
 *   @tree: The tree.
 *   &returns: The instance or null.
 */
struct avltree_inst_t *avltree_last(struct avltree_t *tree)
{
	struct avltree_node_t *node;

	node = avltree_root_last(&tree->root);
	return node ? getparent(node, struct avltree_inst_t, node): NULL;
}

/**
 * Retrieve the previous instance from the tree.
 *   @inst: The current instance.
 *   &returns: The previous instance or null.
 */
struct avltree_inst_t *avltree_prev(struct avltree_inst_t *inst)
{
	struct avltree_node_t *node;

	node = avltree_node_prev(&inst->node);
	return node ? getparent(node, struct avltree_inst_t, node): NULL;
}

/**
 * Retrieve the next instance from the tree.
 *   @inst: The current instance.
 *   &returns: The next instance or null.
 */
struct avltree_inst_t *avltree_next(struct avltree_inst_t *inst)
{
	struct avltree_node_t *node;

	node = avltree_node_next(&inst->node);
	return node ? getparent(node, struct avltree_inst_t, node): NULL;
}
