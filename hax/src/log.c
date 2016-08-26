#include "common.h"


/*
 * local declarations
 */
static bool log_init = false;
static const char *log_path = "log";
static struct sys_mutex_t log_lock = SYS_MUTEX_INIT;


/**
 * Log data.
 *   @path: The path.
 *   @line: The line.
 *   @format: The format.
 *   @...: The printf-style arguments.
 */
void hax_logf(const char *path, unsigned int line, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	hax_logfv(path, line, format, args);
	va_end(args);
}

/**
 * Log data.
 *   @path: The path.
 *   @line: The line.
 *   @format: The format.
 *   @args: The printf-style arguments.
 */
void hax_logfv(const char *path, unsigned int line, const char *format, va_list args)
{
	FILE *file;

	sys_mutex_lock(&log_lock);

	if(!log_init) {
		remove(log_path);
		log_init = true;
	}

	file = fopen(log_path, "a");
	fprintf(file, "%s:%u: ", path, line);
	vfprintf(file, format, args);
	fclose(file);

	sys_mutex_unlock(&log_lock);
}
