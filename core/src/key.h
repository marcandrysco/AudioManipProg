#ifndef KEY_H
#define KEY_H

/**
 * Mode enumerator.
 *   @amp_mode_ionian_v: Ionian.
 *   @amp_mode_aeolian_v: Aeolian
 *   @amp_mode_maj_v: Alternate name for ionian.
 */
enum amp_mode_e {
	amp_mode_ionian_v = 1,
	amp_mode_maj_v = 1,
	amp_mode_aeolian_v = 6,
	amp_mode_min_v = 6,
};


/*
 * key declarations
 */
int16_t amp_key_get(const char *str, char **endptr);
char *amp_key_note(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_key_str(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_key_freq(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);

/*
 * letter declarations
 */
int8_t amp_let_get(const char *str, char **endptr);
const char *amp_let_str(int16_t let);

/*
 * mode declarations
 */
int16_t amp_ival_maj(int16_t off);
int16_t amp_ival_min(int16_t off);

uint8_t amp_scale_maj(uint8_t root, int16_t off);
uint8_t amp_scale_min(uint8_t root, int16_t off);

char *amp_mode_ival(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);
char *amp_mode_scale(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env);


/**
 * Calculate a key from a frequency.
 *   @freq: The frequency.
 *   @a: Concert A pitch.
 *   &returns: The note as a double.
 */
static inline double amp_key_calc(double freq, double a)
{
	return 12.0 * fmod(fmod(1.0 + log2(freq / a), 1.0) + 1.0, 1.0);
}

/**
 * Round a double key to the nearest integer float.
 *   @key: The double key.
 *   @diff: Optional. Stored the difference from nearest key.
 *   &returns: The integer key.
 */
static inline uint8_t amp_key_nearest(double key, double *diff)
{
	uint8_t let;

	let = ((uint8_t)round(key) % 12 + 12) % 12;

	if(diff) {
		*diff = key - let;
		if(*diff > 11.0)
			*diff -= 12.0;
	}

	return let;
}


/**
 * Calculate the value from the octave and letter.
 *   @octave: The octave.
 *   @letter: The letter.
 *   &returns: The key.
 */
static inline int16_t amp_key_val(int8_t octave, uint8_t letter)
{
	return (int16_t)octave * 12 + letter;
}

/**
 * Retrieve a frequency for a note as a float.
 *   @note: The note.
 *   &returns: The frequency.
 */
static inline double amp_key_freq_f(uint8_t note)
{
	return 27.5f * powf(2.0f, ((float)note - 9.0f) / 12.0f);
}

/**
 * Retrieve a frequency for a note as a double.
 *   @note: The note.
 *   &returns: The frequency.
 */
static inline double amp_key_freq_d(uint8_t note)
{
	return 27.5f * powf(2.0f, ((double)note - 9.0f) / 12.0f);
}

#endif
