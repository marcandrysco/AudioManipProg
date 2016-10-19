File System
===========



    bool fs_exists(const char *path)

The `fs_exists` function check is a file path exists.

### Directory Creation -- `fs_mkdir`, `fs_trymkdir`

    char *fs_mkdir(const char *path, uint16_t perm)
    bool fs_trymkdir(const char *path, uint16_t perm)

The `fs_mkdir` function creates a directory at `path` with `perm` permissions.
The function returns the error message on failure.

The `fs_trymkdir` function attempts to create the directory, returning  true`
if successful.

## Windows Path Manipulation

    int w32_unix2win(wchar_t *restrict out, const char *restrict in);
    int w32_win2unix(char *restrict out, const wchar_t *restrict in);
