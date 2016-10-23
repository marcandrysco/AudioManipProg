#ifndef PRINT_H
#define PRINT_H

/*
 * printing declarations
 */
size_t hax_lprintf(const char *restrict format, ...);
size_t hax_vlprintf(const char *restrict format, va_list args);

char *hax_mprintf(const char *restrict format, ...);
char *hax_vmprintf(const char *restrict format, va_list args);

void hax_hprintf(struct io_file_t file, const char *restrict format, ...);
void hax_vhprintf(struct io_file_t file, const char *restrict format, va_list args);;
void hax_vhprintf_custom(struct io_file_t file, const struct io_print_t *print, const char *restrict format, va_list args);

void hax_fprintf(FILE *file, const char *restrict format, ...);
void hax_vfprintf(FILE *file, const char *restrict format, va_list args);

void hax_printf(const char *restrict format, ...);
void hax_vprintf(const char *restrict format, va_list args);

noreturn void hax_vfatal(const char *restrict format, va_list args);
noreturn void hax_vfatal_dbg(const char *file, unsigned int line, const char *restrict format, va_list args);
noreturn void hax_fatal(const char *restrict format, ...);
noreturn void hax_fatal_dbg(const char *file, unsigned int line, const char *restrict format, ...);


/**
 * Direct 'vprintf' call.
 *   @format: The format.
 *   @args: The arguments.
 *   &returns: Number of bytes written.
 */
static inline int d_vprintf(const char *restrict format, va_list args)
{
	return vprintf(format, args);
}

/**
 * Direct 'printf' call.
 *   @format: The format.
 *   @args: The arguments.
 *   &returns: Number of bytes written.
 */
static inline size_t d_printf(const char *restrict format, ...)
{
	int retval;
	va_list args;

	va_start(args, format);
	retval = vprintf(format, args);
	va_end(args);

	return retval;
}

/**
 * Direct 'fprintf' call.
 *   @file: The file.
 *   @format: The format.
 *   @args: The arguments.
 *   &returns: Number of bytes written.
 */
static inline int d_fprintf(FILE *file, const char *restrict format, ...)
{
	int retval;
	va_list args;

	va_start(args, format);
	retval = vfprintf(file, format, args);
	va_end(args);

	return retval;
}

/**
 * Direct 'vfprintf' call.
 *   @file: The file.
 *   @format: The format.
 *   @args: The arguments.
 *   &returns: Number of bytes written.
 */
static inline int d_vfprintf(FILE *file, const char *restrict format, va_list args)
{
	return vfprintf(file, format, args);
}

static inline size_t d_vsprintf(char *restrict str, const char *restrict format, va_list args)
{
	return vsprintf(str, format, args);
}
static inline size_t d_vsnprintf(char *restrict str, size_t nbytes, const char *restrict format, va_list args)
{
	return vsnprintf(str, nbytes, format, args);
}


/*
 * short macros
 */
#define lprintf hax_lprintf
#define vlprintf hax_vlprintf

#define mprintf hax_mprintf
#define vmprintf hax_vmprintf

#define hprintf hax_hprintf
#define vhprintf hax_vhprintf

#define fprintf hax_fprintf
#define vfprintf hax_vfprintf

#define printf hax_printf
#define vprintf hax_vprintf

#ifdef DEBUG
#	define fatal(...) hax_fatal_dbg(__FILE__, __LINE__, __VA_ARGS__)
#	define vfatal(...) hax_vfatal_dbg(__FILE__, __LINE__, __VA_ARGS__)
#else
#	define fatal hax_fatal
#	define vfatal hax_vfatal
#endif

#endif
