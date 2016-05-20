#include "common.h"


/**
 * Scan a key from a string.
 *   @str: The string.
 *   @endptr: The end pointer.
 *   &returns: The letter value or negative.
 */
int16_t amp_key_get(const char *str, char **endptr)
{
	long octave;
	int8_t let;

	let = amp_let_get(str, (char **)&str);
	if(let < 0)
		return let;

	errno = 0;
	octave = strtol(str, (char **)&str, 0);
	if((octave == 0) && (errno != 0))
		return -1;

	if(endptr != NULL)
		*endptr = (char *)str;

	return amp_key_val(octave, let);
}


/**
 * Evaluate a key value.
 *   @value: The value.
 *   @env: The environment.
 *   @err: The error.
 *   &returns: The value or null.
 */
struct ml_value_t *amp_key_eval(struct ml_value_t *value, struct ml_env_t *env, char **err)
{
	int16_t key;
	char *endptr;

	if(value->type == ml_value_str_v)
		key = amp_key_get(value->data.str, &endptr);
	else
		key = -1;

	if((key < 0) || (*endptr != '\0'))
		*err = amp_printf("Invalid key '%s'.", value->data.str);

	ml_value_delete(value);

	return (*err == NULL) ? ml_value_num(key, ml_tag_copy(value->tag)) : NULL;
}


/**
 * Scan a letter from a string.
 *   @str: The string.
 *   @endptr: The end pointer.
 *   &returns: The letter value or negative.
 */
int8_t amp_let_get(const char *str, char **endptr)
{
	uint8_t val;

	switch(*str++) {
	case 'A': val = 0; break;
	case 'B': val = 2; break;
	case 'C': val = 3; break;
	case 'D': val = 5; break;
	case 'E': val = 7; break;
	case 'F': val = 8; break;
	case 'G': val = 10; break;
	default: return -1;
	}

	if(*str == 'b')
		val = (val + 11) % 12, str++;
	else if(*str == '#')
		val = (val + 1) % 12, str++;

	if(endptr != NULL)
		*endptr = (char *)str;

	return val;
}

/**
 * Retrieve the string associated with a letter value.
 *   @let: The letter value.
 *   &returns: The letter or null.
 */
const char *amp_let_str(int16_t let)
{
	switch((let % 12 + 12) % 12) {
	case 0: return "A";
	case 1: return "Bb";
	case 2: return "B";
	case 3: return "C";
	case 4: return "C#";
	case 5: return "D";
	case 6: return "Eb";
	case 7: return "E";
	case 8: return "F";
	case 9: return "F#";
	case 10: return "G";
	case 11: return "G#";
	}

	return NULL;
}
