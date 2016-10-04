Argument Vector Helpers
=======================

The agument vector functions, prefixed with `argv_`, are helper functions for
constructing or parsing argument lists.


## String Functions

    char *argv_serial(char *const *argv)

The `argv_serial` function serializes an argument list into a string.
Parsing the string with `argv_parse` will return the input argument list.

    void argv_parse(const char *str, char ***argv, unsigned int *argc)
    void argv_free(char **argv)

The `argv_parse` function takes an input tring and generates the
corresponding argument list. The list is stored in the reference `argv`, and
the number of elements is optionally stored in `argc`. The list of lists
stored by `argv` should be freed using the `argv_free` function

The `argv_free` function deletes the argument list.
