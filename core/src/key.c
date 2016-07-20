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
 * Evaluate a key string to note value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_key_note(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	uint8_t key;
	char *endptr;

	if(value->type != ml_value_str_v)
		return mprintf("%C: Invalid key string.", ml_tag_chunk(&value->tag));

	key = amp_key_get(value->data.str, &endptr);
	if(*endptr != '\0')
		return mprintf("%C: Invalid key stirng '%s'.", value->data.str, ml_tag_chunk(&value->tag));

	*ret = ml_value_num(key, ml_tag_copy(value->tag));
	return NULL;
}

/**
 * Evaluate a key name value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_key_str(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	int key;

	if(value->type != ml_value_num_v)
		return mprintf("%C: Invalid key value.", ml_tag_chunk(&value->tag));

	key = value->data.num;
	if((key < 0) || (key > 255))
		return mprintf("%C: Invalid key number. Must be between 0 and 255.", ml_tag_chunk(&value->tag));

	*ret = ml_value_str(mprintf("%s%u", amp_let_str(key), key / 12), ml_tag_copy(value->tag));
	return NULL;
}

/**
 * Evaluate a key name value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_key_freq(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	int key;

	if(value->type != ml_value_num_v)
		return mprintf("%C: Invalid key value.", ml_tag_chunk(&value->tag));

	key = value->data.num;
	if((key < 0) || (key > 255))
		return mprintf("%C: Invalid key number. Must be between 0 and 255.", ml_tag_chunk(&value->tag));

	*ret = ml_value_flt(amp_key_freq_d(key), ml_tag_copy(value->tag));
	return NULL;
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
	case 'C': val = 0; break;
	case 'D': val = 2; break;
	case 'E': val = 4; break;
	case 'F': val = 5; break;
	case 'G': val = 8; break;
	case 'A': val = 10; break;
	case 'B': val = 12; break;
	default: return -1;
	}

	if(*str == 'b') {
		val = (val + 11) % 12, str++;
		if(*str == 'b')
			val = (val + 11) % 12, str++;
	}
	else if(*str == '#') {
		val = (val + 1) % 12, str++;
		if(*str == '#')
			val = (val + 1) % 12, str++;
	}

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
	case 0: return "B";
	case 1: return "C";
	case 2: return "C#";
	case 3: return "D";
	case 4: return "Eb";
	case 5: return "E";
	case 6: return "F";
	case 7: return "F#";
	case 8: return "G";
	case 9: return "Ab";
	case 10: return "A";
	case 11: return "Bb";
	}

	return NULL;
}


static inline uint16_t dsp_mod_i16(int16_t val, uint16_t mod)
{
	return ((val % mod) + mod) % mod;
}
static inline int16_t dsp_div_i16(int16_t val, uint16_t div)
{
	return (val < 0) ? ((val + 1) / div - 1) : (val / div);
	return (val / div) - ((val < 0) ? 1 : 0);
}

/**
 * Compute the interval from a major root.
 *   @off: The offset.
 *   &returns: The semitone offset.
 */
int16_t amp_ival_maj(int16_t off)
{
	int16_t rem = 12 * dsp_div_i16(off, 7);

	switch(dsp_mod_i16(off, 7)) {
	case 0: return 0 + rem;
	case 1: return 2 + rem;
	case 2: return 4 + rem;
	case 3: return 5 + rem;
	case 4: return 7 + rem;
	case 5: return 9 + rem;
	case 6: return 11 + rem;
	}

	fatal("Unreachable code.");
}

/**
 * Compute the interval from a minor root.
 *   @off: The offset.
 *   &returns: The semitone offset.
 */
int16_t amp_ival_min(int16_t off)
{
	int16_t rem = 12 * dsp_div_i16(off, 7);

	switch(dsp_mod_i16(off, 7)) {
	case 0: return 0 + rem;
	case 1: return 2 + rem;
	case 2: return 3 + rem;
	case 3: return 5 + rem;
	case 4: return 7 + rem;
	case 5: return 8 + rem;
	case 6: return 10 + rem;
	}

	fatal("Unreachable code.");
}


/**
 * Compute a note in a major scale.
 *   @root: The root.
 *   @off: The offset.
 *   &returns: The note.
 */
uint8_t amp_scale_maj(uint8_t root, int16_t off)
{
	return root + amp_ival_maj(off);
}

/**
 * Compute a note in a minor scale.
 *   @root: The root.
 *   @off: The offset.
 *   &returns: The note.
 */
uint8_t amp_scale_min(uint8_t root, int16_t off)
{
	return root + amp_ival_min(off);
}


/**
 * Compute an interval from an value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_mode_ival(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	int mode, off;
	int16_t ival = 0;

	chkret(amp_match_unpack(value, "(d,d)", &mode, &off));

	if((mode < 1) || (mode > 7))
		return mprintf("Invalid mode. Mode must be between 1 and 7.");
	else if((off < -255) || (off > 255))
		return mprintf("Invalid offset. Mode must be between -255 and 255.");

	switch(mode) {
	case amp_mode_maj_v: ival = amp_ival_maj(off); break;
	case amp_mode_min_v: ival = amp_ival_min(off); break;
	}

	*ret = ml_value_num(ival, ml_tag_copy(value->tag));

	return NULL;
}

/**
 * Compute a scaled note from an value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *amp_mode_scale(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	int mode, root, off;
	int16_t ival = 0;

	chkret(amp_match_unpack(value, "(d,d,d)", &mode, &root, &off));

	if((mode < 1) || (mode > 7))
		return mprintf("Invalid mode. Mode must be between 1 and 7.");
	else if((root < 0) || (root > 255))
		return mprintf("Invalid root. Mode must be between 0 and 255.");
	else if((off < -255) || (off > 255))
		return mprintf("Invalid offset. Mode must be between -255 and 255.");

	switch(mode) {
	case amp_mode_maj_v: ival = amp_scale_maj(root, off); break;
	case amp_mode_min_v: ival = amp_scale_min(root, off); break;
	}

	*ret = ml_value_num(ival, ml_tag_copy(value->tag));

	return NULL;
}
