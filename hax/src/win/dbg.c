#include "../common.h"

/*
 * local declarations
 */
static void errstr_proc(struct io_file_t file, void *arg);


/**
 * Create a chunk for a socket error message.
 *   &returns: The chunk.
 */
struct io_chunk_t w32_errstr(void)
{
	return (struct io_chunk_t){ errstr_proc, NULL };
}
static void errstr_proc(struct io_file_t file, void *arg)
{
	char str[1024];

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), str, sizeof(str), NULL);

	if(strlen(str) >= 3)
		str[strlen(str) - 3] = '\0';

	hprintf(file, "%s", str);
}
