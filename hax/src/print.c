#include "common.h"


/**
 * Determine the size for printing.
 *   @format: The printf-style format.
 *   @...: The printf-style argument list.
 *   &returns: The size.
 */
size_t hax_lprintf(const char *restrict format, ...)
{
	size_t len;
	va_list args;

	va_start(args, format);
	len = hax_vlprintf(format, args);
	va_end(args);

	return len;
}

/**
 * Determine the size for printing.
 *   @format: The printf-style format.
 *   @args: The printf-style argument list.
 *   &returns: The size.
 */
size_t hax_vlprintf(const char *restrict format, va_list args)
{
	size_t len = 0;
	struct io_file_t file;

	file = io_file_len(&len);
	hax_vhprintf(file, format, args);
	io_file_close(file);

	return len;
}


/**
 * Print to an allocated string.
 *   @format: The printf-style format.
 *   @...: The printf-style argument list.
 *   &returns: The allocated string.
 */
char *hax_mprintf(const char *restrict format, ...)
{
	char *str;
	va_list args;

	va_start(args, format);
	str = hax_vmprintf(format, args);
	va_end(args);

	return str;
}

/**
 * Print to an allocated string.
 *   @format: The printf-style format.
 *   @args: The printf-style argument list.
 *   &returns: The allocated string.
 */
char *hax_vmprintf(const char *restrict format, va_list args)
{
	char *str;
	struct io_file_t file;

	file = io_file_accum(&str);
	hax_vhprintf(file, format, args);
	io_file_close(file);

	return str;
}

/**
 * Print to a hax file.
 *   @file: The output file.
 *   @format: The printf-style format.
 *   @...: The printf-style argument list.
 */
void hax_hprintf(struct io_file_t file, const char *restrict format, ...)
{
	va_list args;

	va_start(args, format);
	hax_vhprintf(file, format, args);
	va_end(args);
}

/**
 * Print to a hax file.
 *   @file: The output file.
 *   @format: The printf-style format.
 *   @args: The printf-style argument list.
 */
void hax_vhprintf(struct io_file_t file, const char *restrict format, va_list args)
{
	hax_vhprintf_custom(file, io_format_def, format, args);
	/*
	const char *ptr;

	while((ptr = strchr(format, '%')) != NULL) {
		io_file_write(file, format, (ptr - format));

		format = ptr++;
		if(*ptr == 'C')
			io_chunk_proc(va_arg(args, struct io_chunk_t), file);
		else if(*ptr == '%')
			io_file_write(file, ptr, 1);
		else {
			ptr = strpbrk(ptr, "diouxXfFeEgGaAscpn");
			if(ptr == NULL)
				fatal("Invalid printf format '%s'.", format);

			{
				size_t len;
				va_list copy;
				char tmp[ptr-format+2];

				memcpy(tmp, format, ptr-format+1);
				tmp[ptr-format+1] = '\0';

				va_copy(copy, args);
				len = d_vsnprintf(NULL, 0, tmp, copy);
				va_end(copy);

				{
					char buf[len+1];

					d_vsnprintf(buf, len+1, tmp, args);
					io_file_write(file, buf, strlen(buf));
				}
			}
		}

		format = ptr + 1;
	}

	io_file_write(file, format, strlen(format));
	*/
}

/**
 * Print to a hax file with a custom set of printing options.
 *   @file: The output file.
 *   @format: The printf-style format.
 *   @args: The printf-style argument list.
 */
void hax_vhprintf_custom(struct io_file_t file, const struct io_print_t *print, const char *restrict format, va_list args)
{
	size_t i;
	struct arglist_t arglist;
	
	va_copy(arglist.args, args);

	while(*format != '\0') {
		if(*format == '%') {
			format++;

			if(*format != '%') {
				struct io_print_mod_t mod;
				const struct io_print_t *search;

				mod.flags = 0;
				if(*format == '-')
					mod.neg = true, format++;
				else
					mod.neg = false;

				if(*format == '0')
					mod.zero = true, format++;
				else
					mod.zero = false;

				mod.width = mod.prec = 0;
				if(*format != '*') {
					while(isdigit(*format))
						mod.width = mod.width * 10 + *format - '0', format++;
				}
				else
					(mod.flags |= io_print_width_v), format++;

				if(*format == '.') {
					format++;
					if(*format != '*') {
						while(isdigit(*format))
							mod.prec = mod.prec * 10 + *format - '0', format++;
					}
					else
						(mod.flags |= io_print_prec_v), format++;
				}

				if(*format == 'l')
					(mod.flags |= io_print_long_v), format++;
				else if(*format == 'z')
					(mod.flags |= io_print_sizet_v), format++;

				if(*format == ':') {
					//char name[16];
					fatal("stub");
				}
				else {
					search = print;
					while(search->ch != *format) {
						if(search->callback == NULL)
							fatal("Invalid printf specifier '%c'.", *format);

						search++;
					}
				}

				search->callback(file, &mod, &arglist);
			}
			else
				io_file_write(file, "%", 1);

			format++;
		}
		else {
			i = 0;
			do
				i++;
			while((format[i] != '%') && (format[i] != '\0'));

			io_file_write(file, format, i);
			format += i;
		}
	}

	va_end(arglist.args);
}


/**
 * Print to a file. 
 *   @file: The file.
 *   @format: The printf-style format.
 *   @...: The printf-style argument list.
 */
void hax_fprintf(FILE *file, const char *restrict format, ...)
{
	va_list args;

	va_start(args, format);
	hax_vhprintf(io_file_wrap(file), format, args);
	va_end(args);
}

/**
 * Print to a file. 
 *   @file: The file.
 *   @format: The printf-style format.
 *   @args: The printf-style argument list.
 */
void hax_vfprintf(FILE *file, const char *restrict format, va_list args)
{
	hax_vhprintf(io_file_wrap(file), format, args);
}


/**
 * Print to a standard out. 
 *   @file: The file.
 *   @format: The printf-style format.
 *   @...: The printf-style argument list.
 */
void hax_printf(const char *restrict format, ...)
{
	va_list args;

	va_start(args, format);
	hax_vhprintf(io_file_wrap(stdout), format, args);
	va_end(args);
}

/**
 * Print to a standard out. 
 *   @file: The file.
 *   @format: The printf-style format.
 *   @args: The printf-style argument list.
 */
void hax_vprintf(const char *restrict format, va_list args)
{
	hax_vhprintf(io_file_wrap(stdout), format, args);
}


/**
 * Fatally exit a program with an error message. This function does not
 * return.
 *   @format: The printf-style format.
 *   @args: The printf-style argument list.
 */
noreturn void hax_vfatal(const char *restrict format, va_list args)
{
	vfprintf(stderr, format, args);
	fputc('\n', stderr);

	abort();
}

/**
 * Fatally exit a program with an error message with line information. This
 * function does not return.
 *   @format: The printf-style format.
 *   @args: The printf-style argument list.
 */
noreturn void hax_vfatal_dbg(const char *file, unsigned int line, const char *restrict format, va_list args)
{
	d_fprintf(stderr, "%s:%u: ", file, line);
	vfprintf(stderr, format, args);
	fputc('\n', stderr);

	abort();
}

/**
 * Fatally exit a program with an error message. This function does not
 * return.
 *   @format: The printf-style format.
 *   @...: The printf-style argument list.
 */
noreturn void hax_fatal(const char *restrict format, ...)
{
	va_list args;

	va_start(args, format);
	hax_vfatal(format, args);
	va_end(args);
}

/**
 * Fatally exit a program with an error message with line information. This
 * function does not return.
 *   @format: The printf-style format.
 *   @...: The printf-style argument list.
 */
noreturn void hax_fatal_dbg(const char *file, unsigned int line, const char *restrict format, ...)
{
	va_list args;

	va_start(args, format);
	hax_vfatal_dbg(file, line, format, args);
	va_end(args);
}
