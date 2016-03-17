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
	size_t len;
	va_list copy;

	va_copy(copy, args);
	len = vsnprintf(NULL, 0, format, copy);
	va_end(copy);

	str = malloc(len + 1);
	vsprintf(str, format, args);

	return str;
}

/**
 * Print to a hax file.
 *   @format: The printf-style format.
 *   @...: The printf-style argument list.
 *   &returns: The allocated string.
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
 *   @format: The printf-style format.
 *   @args: The printf-style argument list.
 *   &returns: The allocated string.
 */
void hax_vhprintf(struct io_file_t file, const char *restrict format, va_list args)
{
	const char *ptr;

	while((ptr = strchr(format, '%')) != NULL) {
		io_file_write(file, format, (ptr - format));

		format = ptr++;
		if(*ptr == 'C')
			io_chunk_proc(va_arg(args, struct io_chunk_t), file);
		else if(*ptr == '%')
			io_file_write(file, ptr, 1);
		else {
			ptr = strpbrk(ptr, "diouxXfFeEgGaAspn");
			if(ptr == NULL)
				fatal("Invalid printf format '%s'.", format);

			{
				size_t len;
				va_list copy;
				char tmp[ptr - format + 2];

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
}


void hax_fprintf(FILE *file, const char *restrict format, ...)
{
	va_list args;

	va_start(args, format);
	hax_vhprintf(io_file_wrap(file), format, args);
	va_end(args);
}

void hax_vfprintf(FILE *file, const char *restrict format, va_list args)
{
	hax_vhprintf(io_file_wrap(file), format, args);
}

void hax_printf(const char *restrict format, ...)
{
	va_list args;

	va_start(args, format);
	hax_vhprintf(io_file_wrap(stdout), format, args);
	va_end(args);
}


/**
 * Fatally exit a program with an error message. This function does not
 * return.
 *   @format: The printf-style format.
 *   @args: The printf-style argument list.
 */
noreturn void hax_vfatal(const char *restrict format, va_list args)
{
	d_vfprintf(stderr, format, args);
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
