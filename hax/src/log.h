#ifndef LOG_H
#define LOG_H

/*
 * log declarations
 */
void hax_logf(const char *path, unsigned int line, const char *format, ...);
void hax_logfv(const char *path, unsigned int line, const char *format, va_list args);

#define hlogf(format, ...) hax_logf(__FILE__, __LINE__, format, ##__VA_ARGS__)
#define vhlogf(format, args) hax_vlogf(__FILE__, __LINE__, format, args)

#endif
