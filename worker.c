#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>

#include "log.h"
#include "list.h"
#include "worker.h"
#include "engine.h"
#include "common.h"
#include "smm/actypes.h"
#include "protocol.h"

static Engine* get_match_engine(WorkerContext* context) {
	if (context->newer->serial > context->current->serial) {
		engine_destroy(context->current);
		context->current = context->newer;
	}
	return context->current;
}

static void send_result(int client, List* result) {
	PROTOCOL_HEADER resp;
	resp.Command = CMD_RESULT;

	char* buffer = malloc(1024);

	int offset = 2;
	do {
		while (offset + 32 < (int)sizeof(buffer) && result != NULL) {
			int size = int_to_dec(FLT_LIST_GET(result, int)[0],
					buffer + offset);
			buffer[offset + size] = ' ';
			offset += size + 1;

			size = int_to_dec(FLT_LIST_GET(result, int)[1],
					buffer + offset);
			buffer[offset + size] = ' ';
			offset += size + 1;

			result = result->next;
		}
		offset = 0;
	} while (result != NULL);
	write(client, &resp, sizeof(PROTOCOL_HEADER));
	write(client, buffer, resp.Length);
}

static void send_error(int client, char* message, int length)
{
	PROTOCOL_HEADER resp;
	resp.Command = CMD_ERROR;
	resp.Length = length;

	write(client, &resp, sizeof(PROTOCOL_HEADER));
	write(client, message, length);
}

static void serve_client(int client, WorkerContext* context) {
	Engine* engine = get_match_engine(context);
	PROTOCOL_HEADER req;
	int n;
	for (;;) {
		n = read(client, &req, sizeof(PROTOCOL_HEADER));

		if (n < 0) {
			log_error("failed to read data from client: %s",
					strerror(errno));
			return;
		} else if (n == 0) {
			break;
		}

		if (req.Command != CMD_TEST) {
			send_error(client, "Bad command!", 12);
			return ;
		}

		AC_ALPHABET_t text[req.Length];

		// TODO: read ...
		n = read(client, text, req.Length);

		//engine_feed_text(engine, text, n / sizeof(text[0]));
		engine_feed_text(engine, text, req.Length);

		List* result = engine_get_result(engine);
		send_result(client, result);
	};
}

void* match_thread(void* p) {
	WorkerContext* context = p;
	while (1) {
		int client;
		read(context->client_pipe[0], &client, sizeof(client));
		serve_client(client, context);
		close(client);
		++context->served_count;
		mpool_reset(context->pool);
	}
	return NULL;
}

void worker_context_init(WorkerContext* context) {
	context->pool = mpool_create();
	context->newer = engine_init(context->pool);
	context->current = context->newer;
	context->served_count = 0;
	context->waiting_count = 0;
	pipe(context->client_pipe);
}
