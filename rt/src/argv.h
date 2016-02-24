#ifndef ARGV_H
#define ARGV_H

/*
 * argv declarations
 */

void argv_parse(const char *str, char ***argv, unsigned int *argc);
void argv_free(char **argv);
char *argv_serial(char **argv);

#endif
