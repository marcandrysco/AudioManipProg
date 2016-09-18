#ifndef ARGV_H
#define ARGV_H

/*
 * argument list declarations
 */
void argv_parse(const char *str, char ***argv, unsigned int *argc);
void argv_free(char **argv);

char *argv_serial(char *const *argv);

#endif
