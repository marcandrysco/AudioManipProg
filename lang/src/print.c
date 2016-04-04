#include "common.h"


void ml_printf(const char *restrict format, ...)
{
	va_list args;

	va_start(args, format);
	ml_vfprintf(stdout, format, args);
	va_end(args);
}

void ml_vprintf(const char *restrict format, va_list args)
{
	ml_vfprintf(stdout, format, args);
}


void ml_fprintf(FILE *file, const char *restrict format, ...)
{
	va_list args;

	va_start(args, format);
	ml_vfprintf(file, format, args);
	va_end(args);
}

void ml_vfprintf(FILE *file, const char *restrict format, va_list args)
{
	while(*format != '\0') {
		if(*format == '%') {
			format++;
			if(*format == '%')
				fputc('%', file);
			else if(*format == 'M') {
				format++;

				if(*format == 'e')
					ml_expr_print(va_arg(args, struct ml_expr_t *), file);
				else if(*format == 'p')
					ml_pat_print(va_arg(args, struct ml_pat_t *), file);
				else if(*format == 'v')
					fprintf(file, "%C", ml_value_chunk(va_arg(args, struct ml_value_t *)));
				else
					fprintf(stderr, "Unknown format 'M%c'.\n", *format), abort();
			}
			else
				fprintf(stderr, "Unknown format '%c'.\n", *format), abort();
		}
		else
			fputc(*format, file);

		format++;
	}
}
