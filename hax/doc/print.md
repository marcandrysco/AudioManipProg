Formatted Printing
==================

## Length Printing

The `lprintf` and `vlprintf` computes the length printing by the arguments,
discarding the output data.

    size_t hax_lprintf(const char *restrict format, ...);
    size_t hax_vlprintf(const char *restrict format, va_list args);


## Memory Printing

The `mprintf` and `vmprintf` prints the data to an allocated string.

    char *hax_mprintf(const char *restrict format, ...);
    char *hax_vmprintf(const char *restrict format, va_list args);


## Handler Printing

## File Printing
