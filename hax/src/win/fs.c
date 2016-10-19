#include "../common.h"



/**
 * Check if a file path exists.
 *   @path: The paht.
 *   &returns: The exists.
 */
bool fs_exists(const char *path)
{
	wchar_t wpath[w32_unix2win(NULL, path) + 1];

	w32_unix2win(wpath, path);

	return GetFileAttributesW(wpath) != INVALID_FILE_ATTRIBUTES;
}


/**
 * Create a directory.
 *   @path: the path.
 *   @perm: The permission.
 *   &returns: Error.
 */
char *fs_mkdir(const char *path, uint16_t perm)
{
	if(!fs_trymkdir(path, perm))
		return mprintf("Failed to create directory '%s'. %C.", path, w32_errstr());

	return NULL;
}

/**
 * Attempt to create a directory.
 *   @path: the path.
 *   @perm: The permission.
 *   &returns: True if successful.
 */
bool fs_trymkdir(const char *path, uint16_t perm)
{
	wchar_t wpath[w32_unix2win(NULL, path) + 1];

	w32_unix2win(wpath, path);

	return CreateDirectoryW(wpath, NULL);
}


size_t w32_utf8to16(wchar_t *restrict out, const char *restrict in)
{
	return MultiByteToWideChar(CP_UTF8, 0, in, -1, out, out ? INT_MAX : 0) - 1;
}
size_t w32_utf8to16len(const char *in)
{
	return MultiByteToWideChar(CP_UTF8, 0, in, -1, NULL, 0) - 1;
}

size_t w32_utf16to8(char *restrict out, const wchar_t *restrict in)
{
	return WideCharToMultiByte(CP_UTF8, 0, in, -1, out, out ? INT_MAX : 0, NULL, NULL) - 1;
}

size_t w32_utf16to8len(const wchar_t *in)
{
	return WideCharToMultiByte(CP_UTF8, 0, in, -1, NULL, 0, NULL, NULL) - 1;
}


/**
 * Convert a unix path to a windows path.
 *   @out: Optional. The output.
 *   @in: The input.
 *   &returns: The number of bytes necessary, or negative on input path.
 */
int w32_unix2win(wchar_t *restrict out, const char *restrict in)
{
	size_t i, len;

	if(strpbrk(in, "<>:\"\\|?*") != NULL)
		return -1;

	if(in[0] == '/') {
		if(!isalpha(in[1]))
			return -1;
		else if((in[2] != '/') && (in[2] != '\0'))
			return -1;

		if(out != NULL) {
			*out++ = in[1];
			*out++ = L':';
			*out++ = L'\\';
		}

		in += (in[2] == '/') ? 3 : 2;
		len = 3;
	}
	else
		len = 0;

	len += w32_utf8to16(out, in);
	if(out != NULL) {
		for(i = 0; i < len; i++) {
			if(out[i] == L'/')
				out[i] = L'\\';
		}
	}

	return len;
}

/**
 * Convert a windows path to a unix path.
 *   @out: Optional. The output.
 *   @in: The input.
 *   &returns: The number of bytes necessary, or negative on input path.
 */
int w32_win2unix(char *restrict out, const wchar_t *restrict in)
{
	size_t i, len;

	if(iswalpha(in[0]) && (in[1] == L':') && (in[2] == L'\\')) {
		if(out != NULL) {
			*out++ = '/';
			*out++ = in[0];
			*out++ = '/';
		}

		in += 3;
		len = 3;
	}
	else
		len = 0;

	len += w32_utf16to8(out, in);
	if(out != NULL) {
		for(i = 0; i < len; i++) {
			if(out[i] == '\\')
				out[i] = '/';
		}
	}

	return len;
}
