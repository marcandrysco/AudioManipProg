#include "../common.h"


/*
 * global variables
 */
struct ml_eval_t ml_eval_table[] = {
	/* airht */
	{ "exp",    ml_eval_exp },
	{ "log",    ml_eval_log },
	{ "powT",   ml_eval_pow },
	{ "round",  ml_eval_round },
	{ "minT",   ml_eval_min },
	{ "maxT",   ml_eval_max },
	{ "boundT", ml_eval_bound },
	/* conv */
	{ "val2str", ml_eval_val2str },
	{ "flt2int", ml_eval_flt2int },
	/* test */
	{ "isint",  ml_eval_isint },
	{ "isflt",  ml_eval_isflt },
	{ "isnum",  ml_eval_isnum },
	{ "islist", ml_eval_islist },
	/* io */
	{ "print",   ml_eval_print },
	{ "println", ml_eval_println },
	/* list */
	{ "seq",     ml_eval_seq },
	{ "seqfT",   ml_eval_seqf },
	{ "mapT",    ml_eval_map },
	{ "foldrT",  ml_eval_foldr },
	/* string */
	{ "strlen", ml_eval_strlen },
	/* end of list */
	{ NULL, NULL }
};

struct ml_curry_t {
	const char *name;
	unsigned int n;
	ml_eval_f func;
};

struct ml_curry_t ml_curry_table[] = {
	{ "map",   2, ml_eval_map },
	{ "foldr", 3, ml_eval_foldr },
	{ "seqf",  2, ml_eval_seqf },
	/* end of list */
	{ NULL, 0, NULL}
};


/**
 * Find an evaluator given an identifier.
 *   @id: The identifier.
 *   &returns: The evaluator or null.
 */
ml_eval_f ml_eval_find(const char *id)
{
	unsigned int i;

	for(i = 0; ml_eval_table[i].id != NULL; i++) {
		if(strcmp(ml_eval_table[i].id, id) == 0)
			return ml_eval_table[i].func;
	}

	return NULL;
}

struct ml_value_t *ml_eval_closure(const char *id)
{
	if(strcmp(id, "pow") == 0)
		return ml_eval_value(ml_eval_pow, 2);
	else if(strcmp(id, "map") == 0)
		return ml_eval_value(ml_eval_map, 2);
	else if(strcmp(id, "foldr") == 0)
		return ml_eval_value(ml_eval_foldr, 3);
	else if(strcmp(id, "seqf") == 0)
		return ml_eval_value(ml_eval_seqf, 2);
	else if(strcmp(id, "(+)") == 0)
		return ml_eval_value(ml_eval_add, 2);
	else if(strcmp(id, "(-)") == 0)
		return ml_eval_value(ml_eval_sub, 2);
	else if(strcmp(id, "(*)") == 0)
		return ml_eval_value(ml_eval_mul, 2);
	else if(strcmp(id, "(/)") == 0)
		return ml_eval_value(ml_eval_div, 2);
	else if(strcmp(id, "(%)") == 0)
		return ml_eval_value(ml_eval_mod, 2);
	else
		return NULL;
}

/**
 * Create a curried closure for a evaluator.
 *   @func: The function.
 *   @n: The number of argument.
 *   &returns: The value.
 */
struct ml_value_t *ml_eval_value(ml_eval_f func, unsigned int n)
{
	unsigned int i;
	struct ml_tuple_t *tuple;
	struct ml_expr_t *left, *right, *app;
	struct ml_pat_t *pat = NULL, **ref = &pat;

	tuple = ml_tuple_new();

	for(i = 0; i < n; i++) {
		ml_tuple_append(tuple, ml_expr_var(mprintf("_%u", i), ml_tag_copy(ml_tag_null)));
		*ref = ml_pat_var(mprintf("_%u", i), ml_tag_copy(ml_tag_null));
		ref = &(*ref)->next;
	}

	left = ml_expr_value(ml_value_eval(func, ml_tag_copy(ml_tag_null)), ml_tag_copy(ml_tag_null));
	right = ml_expr_tuple(tuple, ml_tag_copy(ml_tag_null));
	app = ml_expr_app(ml_app_new(left, right), ml_tag_copy(ml_tag_null));

	return ml_value_closure(ml_closure_new(NULL, NULL, pat, app), ml_tag_copy(ml_tag_null));
}
