#include "../common.h"


/**
 * Attempt to open a dynamic library.
 *   @lib: Ref. The library.
 *   @path: The path.
 *   &returns: Error.
 */
char *sys_dynlib_open(sys_dynlib_t *lib, const char *path)
{
	*lib = sys_dynlib_tryopen(path);
	if(*lib == NULL)
		return mprintf("Cannot open dynamic library '%s'. %C", path, w32_errstr());

	return NULL;
}

/**
 * Attempt to open a dynamic library.
 *   @path: The path.
 *   &returns: The library or null.
 */
sys_dynlib_t sys_dynlib_tryopen(const char *path)
{
	wchar_t wpath[w32_unix2win(NULL, path) + 1];

	w32_unix2win(wpath, path);

	return LoadLibraryW(wpath);
}

/**
 * Close a dynamic library.
 *   @lib: The library.
 */
void sys_dynlib_close(sys_dynlib_t lib)
{
	FreeLibrary(lib);
}


/**
 * Try to find aress of a symbol in a dynamic library.
 *   @lib: The library.
 *   @sym: The symbol.
 *   &returns: The address if found, null otherwise.
 */
void *sys_dynlib_sym(sys_dynlib_t lib, const char *sym)
{
	return GetProcAddress(lib, sym);
}
