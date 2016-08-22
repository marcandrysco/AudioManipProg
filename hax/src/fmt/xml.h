#ifndef FMT_XML_H
#define FMT_XML_H


/**
 * XML node structure.
 *   @tag: The tag.
 *   @attr: The attribute.
 *   @child: The child nodes.
 */
struct xml_node_t {
	char *tag;
	struct xml_attr_t *attr;
	struct xml_child_t *child;
};

/**
 * Child enumerator.
 *   @xml_text_v: Text node.
 *   @xml_node_v: Element node.
 */
enum xml_child_e {
	xml_text_v,
	xml_node_v
};

/**
 * Child data union.
 *   @text: The text value.
 *   @node: The element node.
 */
union xml_child_u {
	char *text;
	struct xml_node_t *node;
};

/**
 * XML child structure.
 *   @type: The type.
 *   @data: The data.
 *   @next: The next child.
 */
struct xml_child_t {
	enum xml_child_e type;
	union xml_child_u data;

	struct xml_child_t *next;
};

/**
 * XML attribute structure.
 *   @key, value: The key and value.
 */
struct xml_attr_t {
	char *key, *value;

	struct xml_attr_t *next;
};


/*
 * xml declarations
 */
struct xml_node_t *xml_node_new(char *tag);
void xml_node_delete(struct xml_node_t *node);

void xml_append_child(struct xml_node_t *node, struct xml_child_t *child);
void xml_append_attr(struct xml_node_t *node, struct xml_attr_t *attr);

struct xml_child_t *xml_child_new(enum xml_child_e type, union xml_child_u data);
void xml_child_delete(struct xml_child_t *child);

struct xml_child_t *xml_child_text(char *text);
struct xml_child_t *xml_child_node(struct xml_node_t *node);

struct xml_attr_t *xml_attr_new(char *key, char *value);
void xml_attr_delete(struct xml_attr_t *attr);

#endif
