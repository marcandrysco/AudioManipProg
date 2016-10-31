#include "common.h"


/**
 * Audit structure.
 *   @serv: The server.
 *   @id: The identifier.
 */
struct web_audit_t {
	struct web_serv_t *serv;
	const char *id;
};


/*
 * local declarations
 */
static void audit_proc(struct io_file_t file, void *arg);


char b64_tab[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * Compute the encoded length of an encoded string, not including the
 * terminating '\0'.
 *   @nbytes: The number of input bytes.
 *   &returns: The number of character necessary.
 */
size_t b64_enclen(size_t nbytes)
{
	return 4 * ((nbytes + 2) / 3);
}

/**
 * Base64 encode a buffer.
 *   @out: The output string.
 *   @in: The input buffer.
 *   @nbytes: The number of bytes.
 */
void b64_enc(char *out, const void *in, size_t nbytes)
{
	uint32_t num;
	const uint8_t *ptr = in;

	while(nbytes >= 3) {
		num = (ptr[0] << 16) | (ptr[1] << 8) | ptr[2];
		ptr += 3, nbytes -= 3;

		*out++ = b64_tab[(num >> 18)];
		*out++ = b64_tab[(num >> 12) & 0x3F];
		*out++ = b64_tab[(num >> 6) & 0x3F];
		*out++ = b64_tab[num & 0x3F];
	}

	if(nbytes == 1) {
		num = ptr[0];
		*out++ = b64_tab[(num >> 2)];
		*out++ = b64_tab[(num << 4) & 0x3F];
		*out++ = '=';
		*out++ = '=';
	}
	else if(nbytes == 2) {
		num = (ptr[0] << 8) | ptr[1];
		*out++ = b64_tab[(num >> 10)];
		*out++ = b64_tab[(num >> 4) & 0x3F];
		*out++ = b64_tab[(num << 2) & 0x3F];
		*out++ = '=';
	}

	*out++ = '\0';
}

/**
 * Create a new audit.
 *   @serv: The server.
 *   @id: The indetifier.
 *   &returns: The audit.
 */
struct web_audit_t *web_audit_new(struct web_serv_t *serv, const char *id)
{
	struct web_audit_t *audit;

	audit = malloc(sizeof(struct web_audit_t));
	audit->serv = serv;
	audit->id = id;

	return audit;
}

/**
 * Create an audit from a value.
 *   @ret: Ref. The returned value.
 *   @value: The value.
 *   @env: The environment.
 *   &returns: Error.
 */
char *web_audit_make(struct ml_value_t **ret, struct ml_value_t *value, struct ml_env_t *env)
{
	if(value->type != ml_value_str_v)
		return mprintf("WebAudit takes a string as input.");

	*ret = amp_pack_effect(amp_effect(web_serv_audit(web_serv, value->data.str), &web_iface_effect));
	return NULL;
}

/**
 * Delete an audit.
 *   @audit: The audit.
 */
void web_audit_delete(struct web_audit_t *audit)
{
	free(audit);
}


/**
 * Process information on an audit.
 *   @audit: The audit.
 *   @info: The information.
 */
void web_audit_info(struct web_audit_t *audit, struct amp_info_t info)
{
}

/**
 * Process data on an audit.
 *   @audit: The audit.
 *   @buf: The buffer.
 *   @time: The time.
 *   @len: The length.
 *   @queue: The action queue.
 *   &returns: The continuation flag.
 */
bool web_audit_proc(struct web_audit_t *audit, double *buf, struct amp_time_t *time, unsigned int len, struct amp_queue_t *queue)
{

	return false;
}


/**
 * Retrieve the information from an audit.
 *   @audit: The audit.
 *   @file: The file.
 */
void web_audit_print(struct web_audit_t *audit, struct io_file_t file)
{
	hprintf(file, "{}");
}

/**
 * Create a chunk for the audit.
 *   @audit: The audit.
 *   &returns: The chunk.
 */
struct io_chunk_t web_audit_chunk(struct web_audit_t *audit)
{
	return (struct io_chunk_t){ audit_proc, audit };
}
static void audit_proc(struct io_file_t file, void *arg)
{
	web_audit_print(arg, file);
}


/**
 * Load an audit from disk.
 *   @audit: The audit.
 *   &returns: Error.
 */
char *web_audit_load(struct web_audit_t *audit)
{
	return NULL;
}

/**
 * Save an audit to disk.
 *   @audit: The audit.
 *   &returns: Error.
 */
char *web_audit_save(struct web_audit_t *audit)
{
	return NULL;
}
