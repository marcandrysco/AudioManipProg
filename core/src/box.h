#ifndef BOX_H
#define BOX_H

/**
 * Boxed type enumerator.
 *   @amp_box_clock_e: Clock.
 *   @amp_box_ctrl_e: Control
 *   @amp_box_ctrl_e: Control
 *   @amp_box_effect_e: Effect.
 *   @amp_box_instr_e: Instrument.
 *   @amp_box_seq_e: Sequencer.
 */
enum amp_box_e {
	amp_box_clock_e,
	amp_box_ctrl_e,
	amp_box_effect_e,
	amp_box_instr_e,
	amp_box_module_e,
	amp_box_seq_e
};

/**
 * Boxed data union.
 *   @clock: The clock.
 *   @ctrl: Control.
 *   @effect: The effect.
 *   @instr: The instrument.
 *   @seq: The seq.
 */
union amp_box_u {
	struct amp_clock_t clock;
	struct amp_ctrl_t *ctrl;
	struct amp_effect_t effect;
	struct amp_instr_t instr;
	struct amp_module_t module;
	struct amp_seq_t seq;
};

/**
 * Boxed value structure.
 *   @type: The type.
 *   @data: The data.
 */
struct amp_box_t {
	enum amp_box_e type;
	union amp_box_u data;
};


/*
 * null declarations
 */
extern struct amp_instr_t amp_instr_null;
extern struct amp_seq_t amp_seq_null;

/*
 * box declarations
 */
extern struct ml_box_i amp_box_iface;

struct amp_box_t *amp_box_new(enum amp_box_e type, union amp_box_u data);
struct amp_box_t *amp_box_copy(struct amp_box_t *box);
void amp_box_delete(struct amp_box_t *box);

struct amp_box_t *amp_box_clock(struct amp_clock_t clock);
struct amp_box_t *amp_box_ctrl(struct amp_ctrl_t *ctrl);
struct amp_box_t *amp_box_effect(struct amp_effect_t effect);
struct amp_box_t *amp_box_instr(struct amp_instr_t instr);
struct amp_box_t *amp_box_module(struct amp_module_t module);
struct amp_box_t *amp_box_seq(struct amp_seq_t seq);

/*
 * unbox declarations
 */
struct amp_param_t *amp_unbox_param(struct ml_value_t *value);

/*
 * packing declarations
 */
struct ml_value_t *amp_pack_clock(struct amp_clock_t clock);
struct ml_value_t *amp_pack_ctrl(struct amp_ctrl_t *ctrl);
struct ml_value_t *amp_pack_effect(struct amp_effect_t effect);
struct ml_value_t *amp_pack_instr(struct amp_instr_t instr);
struct ml_value_t *amp_pack_module(struct amp_module_t module);
struct ml_value_t *amp_pack_seq(struct amp_seq_t seq);

void amp_match_str(char *str, size_t len, const char *format);
char *amp_match_err(const char *format);
char *amp_match_unpack(struct ml_value_t *value, const char *format, ...);

/**
 * Pack a boxed value into a MuseLang value.
 *   @box: The AMP boxed value.
 *   &returns: The MuseLang boxed value.
 */
static inline struct ml_box_t amp_box_pack(struct amp_box_t *box)
{
	return (struct ml_box_t){ box, &amp_box_iface };
}

/**
 * Pack a boxed value into a MuseLang value.
 *   @box: The AMP boxed value.
 *   &returns: The MuseLang value.
 */
static inline struct ml_value_t *amp_box_value(struct amp_box_t *box)
{
	return ml_value_box(amp_box_pack(box));
}

/**
 * Unpack a boxed value from a MuseLang boxed value.
 *   @box: The MuseLange boxed value.
 *   &returns: The AMP boxed value or null.
 */
static inline struct amp_box_t *amp_box_unpack(struct ml_box_t box)
{
	return (box.iface == &amp_box_iface) ? box.ref : NULL;
}

/**
 * Unpack a boxed value from a MuseLang value.
 *   @box: The MuseLange value.
 *   @type: The requested type.
 *   &returns: The AMP boxed value or null.
 */
static inline struct amp_box_t *amp_unbox_value(struct ml_value_t *value, enum amp_box_e type)
{
	struct amp_box_t *box;

	if(value->type != ml_value_box_e)
		return NULL;

	box = amp_box_unpack(value->data.box);
	if(box == NULL)
		return NULL;

	return (box->type == type) ? box : NULL;
}

static inline bool amp_unbox_isparam(struct ml_value_t *value)
{
	return (value->type == ml_value_num_e) || (amp_unbox_value(value, amp_box_ctrl_e) != NULL) || (amp_unbox_value(value, amp_box_module_e) != NULL);
}

#endif
