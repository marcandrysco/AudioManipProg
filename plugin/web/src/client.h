#ifndef CLIENT_H
#define CLIENT_H

/**
 * Client structure.
 *   @id: The identifier.
 *   @tm: The time.
 *   @msg: Thhe message list.
 *   @next: The next client.
 */
struct web_client_t {
	uint32_t id;
	uint64_t tm;
	struct web_msg_t *msg;

	struct web_client_t *next;
};

/**
 * Message structure
 *   @idx: The instance index.
 *   @json: The JSON string.
 *   @next: The next message.
 */
struct web_msg_t {
	unsigned int idx;
	char *json;

	struct web_msg_t *next;
};


/*
 * client declarations
 */
struct web_client_t *web_client_new(uint32_t id);
void web_client_delete(struct web_client_t *client);

void web_client_alive(struct web_client_t *client);
void web_client_put(struct web_client_t *client, unsigned int idx, char *json);

struct web_client_t *web_client_get(struct web_client_t *client, uint32_t id);
uint32_t web_client_add(struct web_client_t **client);
void web_client_clean(struct web_client_t **client);
void web_client_destroy(struct web_client_t *client);

#endif
