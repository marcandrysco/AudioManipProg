#include "../common.h"


/**
 * Create an XML node.
 *   @tag: Consumed. The tag.
 *   &returns: The node.
 */
struct xml_node_t *xml_node_new(char *tag)
{
	struct xml_node_t *node;

	node = malloc(sizeof(struct xml_node_t));
	node->tag = tag;
	node->attr = NULL;
	node->child = NULL;

	return node;
}

/**
 * Delete an XML node.
 *   @node: The node.
 */
void xml_node_delete(struct xml_node_t *node)
{
	free(node->tag);
	free(node);
}


/**
 * Append a child onto a node.
 *   @node: The node.
 *   @child: The child.
 */
void xml_append_child(struct xml_node_t *node, struct xml_child_t *child)
{
	struct xml_child_t **ref = &node->child;

	while(*ref != NULL)
		ref = &(*ref)->next;

	*ref = child;
	child->next = NULL;
}

/**
 * Append an attribute onto a node.
 *   @node: The node.
 *   @attr: The attribute.
 */
void xml_append_attr(struct xml_node_t *node, struct xml_attr_t *attr)
{
	struct xml_attr_t **ref = &node->attr;

	while(*ref != NULL)
		ref = &(*ref)->next;

	*ref = attr;
	attr->next = NULL;
}


/**
 * Create an XML child.
 *   @type: The child type.
 *   @data: The child data.
 *   &returns: The child.
 */
struct xml_child_t *xml_child_new(enum xml_child_e type, union xml_child_u data)
{
	struct xml_child_t *child;

	child = malloc(sizeof(struct xml_child_t));
	child->type = type;
	child->data = data;
	child->next = NULL;

	return child;
}

/**
 * Delete an XML child.
 *   @child: The child.
 */
void xml_child_delete(struct xml_child_t *child)
{
	switch(child->type) {
	case xml_text_v: free(child->data.text); break;
	case xml_node_v: xml_node_delete(child->data.node); break;
	}

	free(child);
}


/**
 * Create a text child.
 *   @text: Consumed. The text.
 *   &returns: The child.
 */
struct xml_child_t *xml_child_text(char *text)
{
	return xml_child_new(xml_text_v, (union xml_child_u){ .text = text });
}

/**
 * Create a node child.
 *   @node: Consumed. The node.
 *   &returns: The child.
 */
struct xml_child_t *xml_child_node(struct xml_node_t *node)
{
	return xml_child_new(xml_node_v, (union xml_child_u){ .node = node });
}


/**
 * Create an XML attribute.
 *   @key: Consumed. The key.
 *   @value: Consumed. The value.
 *   &returns: The attribute.
 */
struct xml_attr_t *xml_attr_new(char *key, char *value)
{
	struct xml_attr_t *attr;

	attr = malloc(sizeof(struct xml_attr_t));
	attr->key = key;
	attr->value = value;
	attr->next = NULL;

	return attr;
}

/**
 * Delete an XML attribute.
 *   @attr: The attribute.
 */
void xml_attr_delete(struct xml_attr_t *attr)
{
	free(attr->key);
	free(attr->value);
	free(attr);
}
