#include "common.h"


/*
 * local declarations
 */

static void match_init(const char **format, va_list *args);
static void match_clear(const char **format, va_list *args);
static bool match_unpack(struct ml_value_t *value, const char **format, va_list *args);

/*
 * global variables
 */

struct amp_instr_t amp_instr_null = { NULL, NULL };
struct amp_seq_t amp_seq_null = { NULL, NULL };


/**
 * Create a boxed value.
 *   @type: The type.
 *   @data: The data.
 */

struct amp_box_t *amp_box_new(enum amp_box_e type, union amp_box_u data)
{
	struct amp_box_t *box;

	box = malloc(sizeof(struct amp_box_t));
	*box = (struct amp_box_t){ type, data };

	return box;
}

/**
 * Copy a boxed value.
 *   @box: The boxed value.
 *   &returns: The copy.
 */

struct amp_box_t *amp_box_copy(struct amp_box_t *box)
{
	switch(box->type) {
	case amp_box_clock_e: return amp_box_clock(amp_clock_copy(box->data.clock));
	case amp_box_ctrl_e: return amp_box_ctrl(amp_ctrl_copy(box->data.ctrl));
	case amp_box_effect_e: return amp_box_effect(amp_effect_copy(box->data.effect));
	case amp_box_instr_e: return amp_box_instr(amp_instr_copy(box->data.instr));
	case amp_box_module_e: return amp_box_module(amp_module_copy(box->data.module));
	case amp_box_seq_e: return amp_box_seq(amp_seq_copy(box->data.seq));
	}

	fprintf(stderr, "Invalid box type.\n"), abort();
}

/**
 * Delete a boxed value.
 *   @box: The box.
 */

void amp_box_delete(struct amp_box_t *box)
{
	switch(box->type) {
	case amp_box_clock_e: amp_clock_delete(box->data.clock); break;
	case amp_box_ctrl_e: amp_ctrl_delete(box->data.ctrl); break;
	case amp_box_effect_e: amp_effect_delete(box->data.effect); break;
	case amp_box_instr_e: amp_instr_delete(box->data.instr); break;
	case amp_box_module_e: amp_module_delete(box->data.module); break;
	case amp_box_seq_e: amp_seq_delete(box->data.seq); break;
	}

	free(box);
}


/**
 * Create a boxed clock.
 *   @clock: The clock.
 *   &returns: The box.
 */

struct amp_box_t *amp_box_clock(struct amp_clock_t clock)
{
	return amp_box_new(amp_box_clock_e, (union amp_box_u){ .clock = clock });
}

/**
 * Create a boxed control.
 *   @ctrl: The control.
 *   &returns: The box.
 */

struct amp_box_t *amp_box_ctrl(struct amp_ctrl_t *ctrl)
{
	return amp_box_new(amp_box_ctrl_e, (union amp_box_u){ .ctrl = ctrl });
}

/**
 * Create a boxed effect.
 *   @effect: The effect.
 *   &returns: The box.
 */

struct amp_box_t *amp_box_effect(struct amp_effect_t effect)
{
	return amp_box_new(amp_box_effect_e, (union amp_box_u){ .effect = effect });
}

/**
 * Create a boxed instrument.
 *   @instr: The instrument.
 *   &returns: The box.
 */

struct amp_box_t *amp_box_instr(struct amp_instr_t instr)
{
	return amp_box_new(amp_box_instr_e, (union amp_box_u){ .instr = instr });
}

/**
 * Create a boxed module.
 *   @module: The module.
 *   &returns: The box.
 */

struct amp_box_t *amp_box_module(struct amp_module_t module)
{
	return amp_box_new(amp_box_module_e, (union amp_box_u){ .module = module });
}

/**
 * Create a boxed sequencer.
 *   @seq: The sequencer.
 *   &returns: The box.
 */

struct amp_box_t *amp_box_seq(struct amp_seq_t seq)
{
	return amp_box_new(amp_box_seq_e, (union amp_box_u){ .seq = seq });
}


/**
 * Pack a clock into a value.
 *   @clock: The clock.
 *   &returns: The value.
 */

struct ml_value_t *amp_pack_clock(struct amp_clock_t clock)
{
	return amp_box_value(amp_box_clock(clock));
}

/**
 * Pack an effect into a value.
 *   @effect: The effect.
 *   &returns: The value.
 */

struct ml_value_t *amp_pack_effect(struct amp_effect_t effect)
{
	return amp_box_value(amp_box_effect(effect));
}

/**
 * Pack a control into a value.
 *   @ctrl: The control.
 *   &returns: The value.
 */

struct ml_value_t *amp_pack_ctrl(struct amp_ctrl_t *ctrl)
{
	return amp_box_value(amp_box_ctrl(ctrl));
}

/**
 * Pack an instrument into a value.
 *   @instr: The instrument.
 *   &returns: The value.
 */

struct ml_value_t *amp_pack_instr(struct amp_instr_t instr)
{
	return amp_box_value(amp_box_instr(instr));
}

/**
 * Pack a module into a value.
 *   @module: The module.
 *   &returns: The value.
 */

struct ml_value_t *amp_pack_module(struct amp_module_t module)
{
	return amp_box_value(amp_box_module(module));
}

/**
 * Pack a sequencer into a value.
 *   @seq: The sequencer.
 *   &returns: The value.
 */

struct ml_value_t *amp_pack_seq(struct amp_seq_t seq)
{
	return amp_box_value(amp_box_seq(seq));
}


/**
 * Unbox a parameter.
 *   @value: The value.
 *   &returns: The parameter or null.
 */

struct amp_param_t *amp_unbox_param(struct ml_value_t *value)
{
	struct amp_box_t *box;

	if(value->type == ml_value_num_e)
		return amp_param_flt(value->data.num);
	else if((box = amp_unbox_value(value, amp_box_ctrl_e)) != NULL)
		return amp_param_ctrl(amp_ctrl_copy(box->data.ctrl));
	else if((box = amp_unbox_value(value, amp_box_module_e)) != NULL)
		return amp_param_module(amp_module_copy(box->data.module));
	else
		return NULL;
}

void match_str(char **str, size_t *len, const char **format)
{
	switch(**format) {
	case 's': snprintf(*str, *len, "string"); break;
	case 'd': snprintf(*str, *len, "int"); break;
	case 'f': snprintf(*str, *len, "num"); break;
	case 'e':
		  (*format)++;
		  switch(**format) {
		  case 'O': snprintf(*str, *len, "osc-type"); break;
		  }

		  break;

	case 'I': snprintf(*str, *len, "Instr"); break;
	case 'E': snprintf(*str, *len, "Effect"); break;
	case 'M': snprintf(*str, *len, "Module"); break;
	case 'P': snprintf(*str, *len, "Param"); break;
		
	case '(':
		(*len)--, (*format)++, *(*str)++ = '(';
		match_str(str, len, format);

		while(**format == ',') {
			(*len)--, (*format)++, *(*str)++ = ',';
			match_str(str, len, format);
		}

		assert(**format == ')');
		(*len)--, (*format)++, *(*str)++ = ')';

		return;
	}

	(*format)++;
	*len -= strlen(*str);
	*str += strlen(*str);
}

void amp_match_str(char *str, size_t len, const char *format)
{
	match_str(&str, &len, &format);
	*str = '\0';

	assert(*format == '\0');
}

/**
 * Create a type error message from the format.
 *   @format: The format.
 *   &returns: The allocated error message.
 */

char *amp_match_err(const char *format)
{
	char msg[256];

	amp_match_str(msg, sizeof(msg), format);

	return amp_printf("Type error. Expected '%s'.", msg);
}

/**
 * Perform an matched unpacking.
 *   @value: The value.
 *   @format; The match format.
 *   @...: The arguments.
 *   &returns: The error on failure.
 */

char *amp_match_unpack(struct ml_value_t *value, const char *format, ...)
{
	bool suc;
	va_list args;
	const char *copy;

	copy = format;
	va_start(args, format);
	match_init(&copy, &args);
	va_end(args);

	copy = format;
	va_start(args, format);
	suc = match_unpack(value, &copy, &args);
	va_end(args);

	ml_value_delete(value);

	if(suc)
		return NULL;

	copy = format;
	va_start(args, format);
	match_clear(&copy, &args);
	va_end(args);

	return amp_match_err(format);
}

/**
 * Initialize a match.
 *   @format; The format.
 *   @args: The arguments.
 */

static void match_init(const char **format, va_list *args)
{
	switch(**format) {
	case 'e':
		(*format)++;
	case 'd':
		*va_arg(*args, int *) = 0;
		break;

	case 'f':
		*va_arg(*args, double *) = 0.0;
		break;

	case 's': *va_arg(*args, char **) = NULL; break;
	case 'E': *va_arg(*args, struct amp_effect_t *) = amp_effect_null; break;
	case 'I': *va_arg(*args, struct amp_instr_t *) = amp_instr_null; break;
	case 'S': *va_arg(*args, struct amp_seq_t *) = amp_seq_null; break;
	case 'P': *va_arg(*args, struct amp_param_t **) = NULL; break;
		
	case '(':
		(*format)++;
		match_init(format, args);

		while(**format == ',') {
			(*format)++;
			match_init(format, args);
		}

		assert(**format == ')');
		(*format)++;

		return;
	}

	(*format)++;
}

/**
 * Clear a match.
 *   @format; The format.
 *   @args: The arguments.
 */

static void match_clear(const char **format, va_list *args)
{
	switch(**format) {
	case 'e':
		(*format)++;
	case 'd':
		va_arg(*args, int *);
		break;
	case 'f':
		va_arg(*args, double *);
		break;

	case 's':
		{
			char *str = *va_arg(*args, char **);

			if(str != NULL)
				free(str);

			break;
		}

	case 'E': amp_effect_erase(*va_arg(*args, struct amp_effect_t *)); break;
	case 'I': amp_instr_erase(*va_arg(*args, struct amp_instr_t *)); break;
	case 'S': amp_seq_erase(*va_arg(*args, struct amp_seq_t *)); break;
	case 'P': amp_param_erase(*va_arg(*args, struct amp_param_t **)); break;
		
	case '(':
		(*format)++;
		match_clear(format, args);

		while(**format == ',') {
			(*format)++;
			match_clear(format, args);
		}

		assert(**format == ')');
		(*format)++;

		return;
	}

	(*format)++;
}

/**
 * Unpack a match.
 *   @value: The value.
 *   @format: The format.
 *   @args: The arguments.
 */

static bool match_unpack(struct ml_value_t *value, const char **format, va_list *args)
{
	if(**format == '(') {
		unsigned int i = 0;
		struct ml_tuple_t tuple;

		if(value->type != ml_value_tuple_e)
			return false;

		tuple = value->data.tuple;
		if(tuple.len == 0)
			return false;

		(*format)++;
		if(!match_unpack(value->data.tuple.value[i++], format, args))
			return false;

		while(**format == ',') {
			if(tuple.len == i)
				return false;

			(*format)++;
			if(!match_unpack(value->data.tuple.value[i++], format, args))
				return false;
		}

		assert(**format == ')');
		(*format)++;

		return true;
	}
	else if(**format == 'P') {
		struct amp_box_t *box;
		struct amp_param_t *param;

		if(value->type == ml_value_num_e)
			param = amp_param_flt(value->data.num);
		else if((box = amp_unbox_value(value, amp_box_ctrl_e)) != NULL)
			param = amp_param_ctrl(amp_ctrl_copy(box->data.ctrl));
		else if((box = amp_unbox_value(value, amp_box_module_e)) != NULL)
			param = amp_param_module(amp_module_copy(box->data.module));
		else
			return false;

		*va_arg(*args, struct amp_param_t **) = param;
		(*format)++;

		return true;
	}
	else if(**format == 'e') {
		int val;

		if(value->type != ml_value_str_e)
			return false;

		(*format)++;
		switch(**format) {
		case 'O': val = amp_osc_type(value->data.str); break;
		default: return false;
		}

		(*format)++;
		*va_arg(*args, int *) = val;

		return (val >= 0);
	}
	else if(**format == 'd') {
		if(value->type == ml_value_num_e)
			*va_arg(*args, int *) = value->data.num;
		else
			return false;

		(*format)++;

		return true;
	}
	else if(**format == 'f') {
		if(value->type == ml_value_num_e)
			*va_arg(*args, double *) = value->data.num;
		else
			return false;

		(*format)++;

		return true;
	}
	else if(**format == 's') {
		if(value->type == ml_value_str_e)
			*va_arg(*args, char **) = strdup(value->data.str);
		else
			return false;

		(*format)++;

		return true;
	}
	else {
		enum amp_box_e type;
		struct amp_box_t *box;

		switch(**format) {
		case 'C': type = amp_box_ctrl_e; break;
		case 'E': type = amp_box_effect_e; break;
		case 'I': type = amp_box_instr_e; break;
		case 'M': type = amp_box_module_e; break;
		case 'S': type = amp_box_seq_e; break;
		default: fprintf(stderr, "Invalid format.\n"), abort();
		}

		box = amp_unbox_value(value, type);
		if(box == NULL)
			return false;

		switch(**format) {
		case 'C': *va_arg(*args, struct amp_ctrl_t **) = amp_ctrl_copy(box->data.ctrl); break;
		case 'E': *va_arg(*args, struct amp_effect_t *) = amp_effect_copy(box->data.effect); break;
		case 'I': *va_arg(*args, struct amp_instr_t *) = amp_instr_copy(box->data.instr); break;
		case 'M': *va_arg(*args, struct amp_module_t *) = amp_module_copy(box->data.module); break;
		case 'S': *va_arg(*args, struct amp_seq_t *) = amp_seq_copy(box->data.seq); break;
		}

		(*format)++;

		return true;
	}
}
