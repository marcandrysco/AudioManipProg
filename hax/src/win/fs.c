#include "../common.h"

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
