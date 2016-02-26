#ifndef PRINT_H
#define PRINT_H

void ml_printf(const char *restrict format, ...);
void ml_vprintf(const char *restrict format, va_list args);

void ml_fprintf(FILE *file, const char *restrict format, ...);
void ml_vfprintf(FILE *file, const char *restrict format, va_list args);

#endif
