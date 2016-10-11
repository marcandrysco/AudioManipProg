#ifndef WIN_FS_H
#define WIN_FS_H

/*
 * path declarations
 */
size_t w32_utf8to16(wchar_t *restrict out, const char *restrict in);
size_t w32_utf8to16len(const char *in);
size_t w32_utf16to8(char *restrict out, const wchar_t *restrict in);
size_t w32_utf16to8len(const wchar_t *in);

int w32_unix2win(wchar_t *restrict out, const char *restrict in);
int w32_win2unix(char *restrict out, const wchar_t *restrict in);

#endif
