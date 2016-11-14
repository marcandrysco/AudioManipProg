#include "common.h"


/*
 * global variables
 */
struct io_print_t io_format_def[] = {
	{ 'd', "decimal",    0, io_format_int },
	{ 'i', "int",        0, io_format_int },
	{ 'u', "uint",       0, io_format_uint },
	{ 'x', "hex",        0, io_format_hex },
	{ 'p', "ptr",        0, io_format_ptr },
	{ 'f', "float",      0, io_format_float },
	{ 'g', "floatshort", 0, io_format_floatshort },
	{ 'e', "floatexp",   0, io_format_floatexp },
	{ 's', "string",     0, io_format_str },
	{ 'c', "char",       0, io_format_char },
	{ 'C', "chunk",      0, io_format_chunk },
	{ '\0', NULL,        0, NULL }
};


/**
 * Printf-style chunk formatter.
 *   @file: The output file.
 *   @mod: The modifier.
 *   @args: The variable argument list with an upcoming string.
 */
void io_format_chunk(struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list)
{
	struct io_chunk_t chunk = va_arg(list->args, struct io_chunk_t);

	if(mod->width > 0) {
		fatal("stub");
		/*
		char pad = ' ';
		size_t len = io_chunk_proc_len(chunk);

		if(mod->width > len) {
			uint16_t i;

			if(neg)
				io_chunk_proc(chunk, file);

			for(i = len; i < mod->width; i++)
				io_file_write(file, &pad, 1);

			if(!neg)
				io_chunk_proc(chunk, file);
		}
		else
			io_chunk_proc(chunk, file);
			*/
	}
	else
		io_chunk_proc(chunk, file);
}

/**
 * Printf-style integer formatter.
 *   @file: The output file.
 *   @mod: The modifier.
 *   @args: The variable argument list with an upcoming string.
 */
void io_format_int(struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list)
{
	io_format_clib('d', file, mod, list);
}

/**
 * Printf-style unsigned integer formatter.
 *   @file: The output file.
 *   @mod: The modifier.
 *   @args: The variable argument list with an upcoming string.
 */
void io_format_uint(struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list)
{
	io_format_clib('u', file, mod, list);
}

/**
 * Printf-style hexidecimal integer formatter.
 *   @file: The output file.
 *   @mod: The modifier.
 *   @args: The variable argument list with an upcoming string.
 */
void io_format_hex(struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list)
{
	io_format_clib('x', file, mod, list);
}

/**
 * Printf-style pointer integer formatter.
 *   @file: The output file.
 *   @mod: The modifier.
 *   @args: The variable argument list with an upcoming string.
 */
void io_format_ptr(struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list)
{
	io_format_clib('p', file, mod, list);
}

/**
 * Printf-style float formatter.
 *   @file: The output file.
 *   @mod: The modifier.
 *   @args: The variable argument list with an upcoming string.
 */
void io_format_float(struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list)
{
	io_format_clib('f', file, mod, list);
}

/**
 * Printf-style float formatter.
 *   @file: The output file.
 *   @mod: The modifier.
 *   @args: The variable argument list with an upcoming string.
 */
void io_format_floatshort(struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list)
{
	io_format_clib('g', file, mod, list);
}

/**
 * Printf-style float formatter.
 *   @file: The output file.
 *   @mod: The modifier.
 *   @args: The variable argument list with an upcoming string.
 */
void io_format_floatexp(struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list)
{
	io_format_clib('e', file, mod, list);
}

/**
 * Printf-style character formatter.
 *   @file: The output file.
 *   @mod: The modifier.
 *   @args: The variable argument list with an upcoming string.
 */
void io_format_char(struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list)
{
	io_format_clib('c', file, mod, list);
}

/**
 * Printf-style string formatter.
 *   @file: The output file.
 *   @mod: The modifier.
 *   @args: The variable argument list with an upcoming string.
 */
void io_format_str(struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list)
{
	io_format_clib('s', file, mod, list);
}

/**
 * Printf-style decimal formatter.
 *   @ch: Format character.
 *   @file: The output file.
 *   @mod: The modifier.
 *   @args: The variable argument list with an upcoming string.
 */
void io_format_clib(char ch, struct io_file_t file, struct io_print_mod_t *mod, struct arglist_t *list)
{
	char format[32];
	va_list args;
	size_t len = 0;

	format[len++] = '%';
	if(mod->neg)
		format[len++] = '-';

	if(mod->zero)
		format[len++] = '0';

	if(mod->flags & io_print_width_v)
		format[len++] = '*';
	else if(mod->width > 0)
		len += sprintf(format + len, "%d", mod->width);

	if(mod->flags & io_print_prec_v)
		len += sprintf(format + len, ".*");
	else if(mod->prec > 0)
		len += sprintf(format + len, ".%d", mod->prec);

	if(mod->flags & io_print_long_v)
		format[len++] = 'l';
	else if(mod->flags & io_print_sizet_v)
#ifdef WINDOWS
		format[len++] = 'l', format[len++] = 'l';
#else
		format[len++] = 'z';
#endif

	format[len++] = ch;
	format[len] = '\0';

	va_copy(args, list->args);
	len = vsnprintf(NULL, 0, format, args);
	va_end(args);

	{
		char buf[len+1];

		va_copy(args, list->args);
		vsprintf(buf, format, args);
		va_end(args);

		io_file_write(file, buf, len);
	}

	if(mod->flags & io_print_width_v)
		va_arg(list->args, int);

	if(mod->flags & io_print_prec_v)
		va_arg(list->args, int);

	switch(ch) {
	case 'c':
	case 'd':
	case 'i':
	case 'u':
	case 'x':
		if(mod->flags & io_print_long_v)
			va_arg(list->args, long);
		else if(mod->flags & io_print_sizet_v)
			va_arg(list->args, size_t);
		else
			va_arg(list->args, int);

		break;

	case 'f':
	case 'g':
	case 'e':
		va_arg(list->args, double);
		break;

	case 'p':
	case 's':
	case 'S':
		va_arg(list->args, void *);
		break;

	case 'C':
		va_arg(list->args, struct io_chunk_t);
		break;

	default:
		fatal("Invalid specifier '%c'.", ch);
	}
}
