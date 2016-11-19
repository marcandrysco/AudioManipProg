#include "common.h"


/**
 * Create a new client.
 *   @id: The identifier.
 *   &returns: The client.
 */
struct web_client_t *web_client_new(uint32_t id)
{
	struct web_client_t *client;

	client = malloc(sizeof(struct web_client_t));
	*client = (struct web_client_t){ id, sys_utime(), NULL, NULL };

	return client;
}

/**
 * Delete a client.
 *   @client: The client.
 */
void web_client_delete(struct web_client_t *client)
{
	struct web_msg_t *msg;

	while(client->msg != NULL) {
		msg = client->msg;
		client->msg = msg->next;

		free(msg->json);
		free(msg);
	}

	free(client);
}


/**
 * Mark the client as alive.
 *   @client: The client.
 */
void web_client_alive(struct web_client_t *client)
{
	client->tm = sys_utime();
}

/**
 * Put a message on the client list.
 *   @client: The client list.
 *   @idx: The index.
 *   @json: Consumed. The JSON string.
 */
void web_client_put(struct web_client_t *client, unsigned int idx, char *json)
{
	struct web_msg_t **msg;

	while(client != NULL) {
		msg = &client->msg;
		while(*msg != NULL)
			msg = &(*msg)->next;

		*msg = malloc(sizeof(struct web_msg_t));
		**msg = (struct web_msg_t){ idx, strdup(json), NULL };

		client = client->next;
	}

	free(json);
}


/**
 * Get a client based on the ID.
 *   @client: The client list.
 *   @id: The identifier.
 *   &returns: The matched client or null.
 */
struct web_client_t *web_client_get(struct web_client_t *client, uint32_t id)
{
	while(client != NULL) {
		if(id == client->id)
			return client;

		client = client->next;
	}

	return NULL;
}

/**
 * Add a client to the client list.
 *   @client: The client list reference.
 *   &returns: The client number
 */
uint32_t web_client_add(struct web_client_t **client)
{
	uint32_t id;

	do
		id = m_rand_u32(NULL);
	while(web_client_get(*client, id) != NULL);

	while(*client != NULL)
		client = &(*client)->next;

	*client = web_client_new(id);
	return id;
}

/**
 * Clean a list of client, deleting timed out clients.
 *   @client: The client list reference.
 */
void web_client_clean(struct web_client_t **client)
{
	uint64_t tm = sys_utime() - 5000000;
	struct web_client_t *tmp;

	while(*client != NULL) {
		if((*client)->tm < tm) {
			tmp = *client;
			*client = tmp->next;

			web_client_delete(tmp);
		}
		else
			client = &(*client)->next;
	}
}

/**
 * Destroy a list of clients.
 *   @client: The list of client.
 */
void web_client_destroy(struct web_client_t *client)
{
	struct web_client_t *tmp;

	while(client != NULL) {
		tmp = client;
		client = tmp->next;

		web_client_delete(tmp);
	}
}
