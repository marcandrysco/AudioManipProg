#include "common.h"


/**
 * Create a new expression.
 *   @type: The type.
 *   @data: The data.
 *   @tag: The tag.
 *   &returns: The expression.
 */

struct ml_expr_t *ml_expr_new(enum ml_expr_e type, union ml_expr_u data, struct ml_tag_t tag)
{
	struct ml_expr_t *expr;

	expr = malloc(sizeof(struct ml_expr_t));
	expr->tag = tag;
	expr->type = type;
	expr->data = data;

	return expr;
}

/**
 * Copy an expression.
 *   @expr: The expression.
 *   &returns: The copy.
 */

struct ml_expr_t *ml_expr_copy(struct ml_expr_t *expr)
{
	switch(expr->type) {
	case ml_expr_id_e:
		return ml_expr_id(strdup(expr->data.id));

	case ml_expr_set_e:
		return ml_expr_set(ml_set_copy(expr->data.set));

	case ml_expr_func_e:
		return ml_expr_func(strdup(expr->data.func.var), ml_expr_copy(expr->data.func.expr));

	case ml_expr_app_e:
		return ml_expr_app(ml_expr_copy(expr->data.app.func), ml_expr_copy(expr->data.app.value));

	case ml_expr_value_e:
		return ml_expr_value(ml_value_copy(expr->data.value));
	}

	fprintf(stderr, "Invalid expression\n"), abort();
}

/**
 * Delete an expression.
 *   @expr: The expression.
 */

void ml_expr_delete(struct ml_expr_t *expr)
{
	switch(expr->type) {
	case ml_expr_id_e:
		free(expr->data.id);
		break;

	case ml_expr_set_e:
		ml_set_delete(expr->data.set);
		break;

	case ml_expr_func_e:
		free(expr->data.func.var);
		ml_expr_delete(expr->data.func.expr);
		break;

	case ml_expr_app_e:
		ml_expr_delete(expr->data.app.func);
		ml_expr_delete(expr->data.app.value);
		break;

	case ml_expr_value_e:
		ml_value_delete(expr->data.value);
		break;
	}

	free(expr);
}


/**
 * Create an identifier expression.
 *   @id: Consumed. The identifier.
 *   &returns: The expression.
 */

struct ml_expr_t *ml_expr_id(char *id)
{
	return ml_expr_new(ml_expr_id_e, (union ml_expr_u){ .id = id }, (struct ml_tag_t){ NULL, 0, 0, 0 });
}

/**
 * Create a set expression.
 *   @set: Consumed. The set.
 *   &returns: The expression.
 */

struct ml_expr_t *ml_expr_set(struct ml_set_t set)
{
	return ml_expr_new(ml_expr_set_e, (union ml_expr_u){ .set = set }, (struct ml_tag_t){ NULL, 0, 0, 0 });
}

/**
 * Create a function expression.
 *   @var: Consumed. The variable.
 *   @expr: Consumed. The expression.
 *   &returns: The expression.
 */

struct ml_expr_t *ml_expr_func(char *var, struct ml_expr_t *expr)
{
	return ml_expr_new(ml_expr_func_e, (union ml_expr_u){ .func = { var, expr } }, (struct ml_tag_t){ NULL, 0, 0, 0 });
}

/**
 * Create an application expression.
 *   @func: Consumed. The function expression.
 *   @value: Consumed. The value expression.
 *   &returns: The expression.
 */

struct ml_expr_t *ml_expr_app(struct ml_expr_t *func, struct ml_expr_t *value)
{
	return ml_expr_new(ml_expr_app_e, (union ml_expr_u){ .app = { func, value } }, (struct ml_tag_t){ NULL, 0, 0, 0 });
}

/**
 * Create a constant value expression.
 *   @value: The constant value.
 *   &returns: The expression.
 */

struct ml_expr_t *ml_expr_value(struct ml_value_t *value)
{
	return ml_expr_new(ml_expr_value_e, (union ml_expr_u){ .value = value }, (struct ml_tag_t){ NULL, 0, 0, 0 });
}


/**
 * Evaluate an expression.
 *   @expr: The expression.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value.
 */

struct ml_value_t *ml_expr_eval(struct ml_expr_t *expr, struct ml_env_t *env, char **err)
{
#undef fail
#define fail(str, ...) do { if(*err == NULL) ml_eprintf(err, "%s:%u:%u: " str, expr->tag.file, expr->tag.line, expr->tag.col, ##__VA_ARGS__); return NULL; } while(0)

	switch(expr->type) {
	case ml_expr_set_e:
		{
			unsigned int i;
			struct ml_tuple_t tuple;
			struct ml_value_t *sub;

			tuple = ml_tuple_new();
			for(i = 0; i < expr->data.set.len; i++) {
				sub = ml_expr_eval(expr->data.set.expr[i], env, err);
				if(sub == NULL)
					break;

				ml_tuple_add(&tuple, sub);
			}

			if(i == expr->data.set.len)
				return ml_value_tuple(tuple);
			else { 
				ml_tuple_delete(tuple);

				return NULL;
			}
		}

	case ml_expr_id_e:
		{
			struct ml_value_t *value;

			value = ml_env_lookup(env, expr->data.id);
			if(value == NULL)
				fail("Unknown variable '%s'.", expr->data.id);

			return ml_value_copy(value);
		}

	case ml_expr_func_e:
		return ml_value_closure(ml_closure(ml_env_copy(env), strdup(expr->data.func.var), NULL, ml_expr_copy(expr->data.func.expr)));

	case ml_expr_app_e:
		{
			struct ml_env_t *sub;
			struct ml_value_t *func, *value;

			func = ml_expr_eval(expr->data.app.func, env, err);
			if(func == NULL)
				return NULL;

			if(func->type == ml_value_closure_e) {
				sub = ml_env_copy(func->data.closure.env);

				value = ml_expr_eval(expr->data.app.value, env, err);
				if(value != NULL) {
					ml_env_add(sub, strdup(func->data.closure.var), value);

					value = ml_expr_eval(func->data.closure.expr, sub, err);
				}

				ml_env_delete(sub);
			}
			else if(func->type == ml_value_impl_e) {
				value = ml_expr_eval(expr->data.app.value, env, err);
				if(value != NULL)
					value = func->data.impl(value, env, err);
			}
			else
				fail("Cannot call non-function value.");

			ml_value_delete(func);

			return value;
		}

	case ml_expr_value_e:
		return ml_value_copy(expr->data.value);
	}

	fprintf(stderr, "Invalid expression type.\n"), abort();
}


/**
 * Print an expression.
 *   @expr: The expression.
 *   @file: The file.
 */

void ml_expr_print(struct ml_expr_t *expr, FILE *file)
{
	switch(expr->type) {
	case ml_expr_id_e:
		fprintf(file, "id(%s)", expr->data.id);
		break;

	case ml_expr_set_e:
		{
			unsigned int i;

			fprintf(file, "set(");
			for(i = 0; i < expr->data.set.len; i++) {
				if(i > 0)
					fprintf(file, ",");

				ml_expr_print(expr->data.set.expr[i], file);
			}
			fprintf(file, ")");
		}
		break;

	case ml_expr_func_e:
		fprintf(file, "func(%s,", expr->data.func.var);
		ml_expr_print(expr->data.func.expr, file);
		fprintf(file, ")");
		break;

	case ml_expr_app_e:
		fprintf(file, "app(");
		ml_expr_print(expr->data.app.func, file);
		fprintf(file, ",");
		ml_expr_print(expr->data.app.value, file);
		fprintf(file, ")");
		break;

	case ml_expr_value_e:
		fprintf(file, "value");
		break;
	}
}


/**
 * Create a set.
 *   &returns: The set.
 */

struct ml_set_t ml_set_new(void)
{
	return (struct ml_set_t){ 0, malloc(0) };
}

/**
 * Create a set with two parameters.
 *   @left: The left expression.
 *   @right: The right expression.
 *   &returns: The set.
 */

struct ml_set_t ml_set_new2(struct ml_expr_t *left, struct ml_expr_t *right)
{
	struct ml_set_t set;

	set.len = 2;
	set.expr = malloc(2 * sizeof(void *));
	set.expr[0] = left;
	set.expr[1] = right;

	return set;
}

/**
 * Copy a set.
 *   @set: The original set.
 *   &returns: The copied set.
 */

struct ml_set_t ml_set_copy(struct ml_set_t set)
{
	unsigned int i;
	struct ml_set_t copy;

	copy.len = set.len;
	copy.expr = malloc(set.len * sizeof(void *));
	for(i = 0; i < set.len; i++)
		copy.expr[i] = ml_expr_copy(set.expr[i]);

	return copy;
}

/**
 * Delete a set.
 *   @set: The set.
 */

void ml_set_delete(struct ml_set_t set)
{
	unsigned int i;

	for(i = 0; i < set.len; i++)
		ml_expr_delete(set.expr[i]);

	free(set.expr);
}

/**
 * Add an value to the set.
 *   @set: The set.
 *   @value: Consume The value.
 */

void ml_set_add(struct ml_set_t *set, struct ml_expr_t *expr)
{
	set->expr = realloc(set->expr, (set->len + 1) * sizeof(void *));
	set->expr[set->len++] = expr;
}
