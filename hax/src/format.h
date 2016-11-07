#ifndef FORMAT_H
#define FORMAT_H

/**
 * Print enumerator.
 *   @io_print_long_v: Long.
 *   @io_print_size_v: SizeT.
 *   @io_print_width_v: Variable width.
 *   @io_print_prec_v: Variable precision.
 */
enum io_print_e {
	io_print_long_v = 0x1,
	io_print_sizet_v = 0x2,
	io_print_width_v = 0x4,
	io_print_prec_v = 0x8,
};

/**
 * Print callback modifiers structure.
 *   @flags;
 *   @zero, neg: Zero padding, and negative flag.
 *   @width, prec: The field width and precision.
 */
struct io_print_mod_t {
	uint16_t flags;
	bool zero, neg;
	uint16_t width, prec;
};

/**
 * Printing callback funtion.
 *   @file: The output file.
 *   @mod: Modifiers.
 *   @args: The variable argument list. Read only the parameter expected.
 */
typedef void (*io_print_f)(struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list);

/**
 * Printing callback structure.
 *   @ch: The short character. Set to '\0' if unused.
 *   @name: The long name. Set to 'NULL' if unused.
 *   @callback: The callback function.
 */
struct io_print_t {
	char ch;
	const char *name;
	uint16_t flags;
	io_print_f callback;
};


/*
 * format declarations
 */
extern struct io_print_t io_format_def[];

void io_format_chunk(struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list);
void io_format_int(struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list);
void io_format_uint(struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list);
void io_format_hex(struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list);
void io_format_ptr(struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list);
void io_format_float(struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list);
void io_format_floatshort(struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list);
void io_format_floatexp(struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list);
void io_format_char(struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list);
void io_format_str(struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list);
void io_format_clib(char ch, struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list);

#endif
