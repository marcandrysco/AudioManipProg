#ifndef WIN_DYNLIB_H
#define WIN_DYNLIB_H


/*
 * dynamic library declaratoins
 */
typedef HMODULE sys_dynlib_t;

char *sys_dynlib_open(sys_dynlib_t *lib, const char *path);
sys_dynlib_t sys_dynlib_tryopen(const char *path);
void sys_dynlib_close(sys_dynlib_t lib);

void *sys_dynlib_sym(sys_dynlib_t lib, const char *sym);

#endif
