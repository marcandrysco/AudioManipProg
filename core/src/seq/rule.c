#include "../common.h"


/**
 * Rule structure.
 *   @head: The head rule.
 */
struct amp_rule_t {
	struct amp_rule_inst_t *head;
};

/**
 * Rule instance structure.
 *   @allow: The allow flag.
 *   @low, high: The low and high range.
 *   @next: The next instance.
 */
struct amp_rule_inst_t {
	bool allow;
	uint16_t low, high;

	struct amp_rule_inst_t *next;
};


/**
 * Create a new rule.
 *   &returns: The rule.
 */
struct amp_rule_t *amp_rule_new(void)
{
	struct amp_rule_t *rule;

	rule = malloc(sizeof(struct amp_rule_t));

	return rule;
}

/**
 * Delete a rule.
 *   @rule: The rule.
 */
void amp_rule_delete(struct amp_rule_t *rule)
{
	free(rule);
}


/**
 * Create a rule from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_rule_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
#define onexit amp_rule_delete(rule);
	struct amp_rule_t *rule;

	rule = amp_rule_new();

	if(value->type == ml_value_list_v)
		;
	else
		fail("%C: Invalid type. Expected single rule or list of rules.");

#undef onexit
	return NULL;
}
