#include "common.h"

/**
 * Initialize a list root.
 *   &returns: The list root.
 */
struct list_root_t list_root_init(void)
{
	return (struct list_root_t){ 0, NULL, NULL };
}

/**
 * Destroy a list root.
 *   @root: The list root.
 *   @offset; The offset.
 *   @delete; The deletion callback.
 */
void list_root_destroy(struct list_root_t *root, ssize_t offset, delete_f delete)
{
	struct list_node_t *cur, *next;

	for(cur = root->head; cur != NULL; cur = next) {
		next = cur->next;
		delete((void *)cur - offset);
	}
}


/**
 * Prepend a node onto the list root.
 *   @root: The root.
 *   @node: The node.
 */
void list_root_prepend(struct list_root_t *root, struct list_node_t *node)
{
	node->next = root->head;
	node->prev = NULL;
	*(root->head ? &root->head->prev : &root->tail) = node;

	root->len++;
	root->head = node;
}

/**
 * Append a node onto the list root.
 *   @root: The root.
 *   @node: The node.
 */
void list_root_append(struct list_root_t *root, struct list_node_t *node)
{
	node->prev = root->tail;
	node->next = NULL;
	*(root->tail ? &root->tail->next : &root->head) = node;

	root->len++;
	root->tail = node;
}


/**
 * Insert a node onto the list root before a cursor.
 *   @root: The root.
 *   @cur: The cursor node.
 *   @node: The node.
 */
void list_root_insert_before(struct list_root_t *root, struct list_node_t *cur, struct list_node_t *node)
{
	if(cur->prev != NULL)
		cur->prev->next = node;
	else
		root->head = node;

	node->next = cur;
	node->prev = cur->prev;
	cur->prev = node;

	root->len++;
}

/**
 * Insert a node onto the list root after a cursor.
 *   @root: The root.
 *   @cur: The cursor node.
 *   @node: The node.
 */
void list_root_insert_after(struct list_root_t *root, struct list_node_t *cur, struct list_node_t *node)
{
	if(cur->next != NULL)
		cur->next->prev = node;
	else
		root->tail = node;

	node->prev = cur;
	node->next = cur->next;
	cur->next = node;

	root->len++;
}

/**
 * Remove a node from the root.
 *   @root: The root.
 *   @node: The node.
 */
void list_root_remove(struct list_root_t *root, struct list_node_t *node)
{
	if(node->next == NULL)
		root->tail = node->prev;
	else
		node->next->prev = node->prev;

	if(node->prev == NULL)
		root->head = node->next;
	else
		node->prev->next = node->next;

	root->len--;
}
