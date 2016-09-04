#include "../common.h"


void json_quote_print(struct io_file_t file, const char *str)
{
}

struct io_chunk_t json_quote(const char *str)
{
	return io_chunk_str(str);
}
