#include "common.h"


bool ml_match_cons(struct ml_value_t *value, struct ml_value_t *head, struct ml_value_t *tail)
{
	if(value->type != ml_value_list_e)
		return false;

	return true;
}


/**
 * Peform a pattern match, adding to an environment.
 *   @env: The environment.
 *   @pat: The pattern.
 *   @value: The value.
 *   &returns: True if successful, false otherwise.
 */

bool ml_match_pat(struct ml_env_t *env, struct ml_expr_t *pat, struct ml_value_t *value)
{
	switch(pat->type) {
	case ml_expr_id_e:
		ml_env_add(env, strdup(pat->data.id), ml_value_copy(value));
		return true;

	case ml_expr_set_e:
		if(value->type != ml_value_tuple_e)
			return false;

		{
			unsigned int i;
			struct ml_tuple_t tuple = value->data.tuple;

			if(tuple.len != pat->data.set.len)
				return false;

			for(i = 0; i < tuple.len; i++) {
				if(!ml_match_pat(env, pat->data.set.expr[i], tuple.value[i]))
					return false;
			}

			return true;
		}

	case ml_expr_app_e:
		if(pat->data.app.func->type != ml_expr_value_e)
			return false;

		{
			struct ml_value_t *func = pat->data.app.func->data.value;

			if(func->type == ml_value_impl_e) {
				if(func->data.impl == ml_eval_cons) {
					struct ml_set_t set;

					if(value->type != ml_value_list_e)
						return false;

					if(value->data.list.head == NULL)
						return false;

					if(pat->data.app.value->type != ml_expr_set_e)
						return false;

					set = pat->data.app.value->data.set;
					if(set.len != 2)
						return false;

					if(!ml_match_pat(env, set.expr[0], value->data.list.head->value))
						return false;

					{
						bool suc;
						struct ml_list_t tail;
						struct ml_value_t *sub;

						tail = ml_list_copy(value->data.list);
						ml_value_delete(ml_list_remove(&tail, tail.tail));

						sub = ml_value_list(tail);
						suc = ml_match_pat(env, set.expr[1], sub);
						ml_value_delete(sub);

						return suc;
					}
				}
				else
					return false;
			}
			else
				return false;
		}
		break;

	case ml_expr_func_e:
	case ml_expr_cond_e:
	case ml_expr_value_e:
	case ml_expr_let_e:
		return false;
	}

	fprintf(stderr, "invalid expression type.\n"), abort();
}
