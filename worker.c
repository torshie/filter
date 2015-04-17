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
	log_info("Sending results");

	PROTOCOL_HEADER resp;
	resp.Command = CMD_RESULT;
	resp.Length = 0;

	char buffer[MAX_DATA_LENGTH+1] = {'\0'};
	RESULT_PAIR rp;

	int offset = 0;
	do {
		while (offset + 32 < (int)sizeof(buffer) && result != NULL) {
			rp.StartPos = FLT_LIST_GET(result, int)[0];
			rp.Length = FLT_LIST_GET(result, int)[1];
			rp.StartPos -= rp.Length;
			memcpy(buffer+offset, &rp, RESULT_PAIR_LENGTH);
			log_info("Size of rp=%d, struct=%d", sizeof(rp), sizeof(RESULT_PAIR));

			offset += RESULT_PAIR_LENGTH;

			result = result->next;
		}

	} while (result != NULL);
	resp.Length = offset;
	log_info("Result data length=%d", resp.Length);

	write(client, &resp, PROTOCOL_HEADER_LENGTH);
	write(client, buffer, resp.Length);
}

static void send_error(int client, char* message, int length)
{
	PROTOCOL_HEADER resp;
	resp.Command = CMD_ERROR;
	resp.Length = length;

	write(client, &resp, PROTOCOL_HEADER_LENGTH);
	write(client, message, length);
}

static void serve_client(int client, WorkerContext* context) {
	Engine* engine = get_match_engine(context);
	PROTOCOL_HEADER req;
	int n;
	for (;;) {
		log_info("Size of Protocol Header is: %d", PROTOCOL_HEADER_LENGTH);

		n = read(client, &req, PROTOCOL_HEADER_LENGTH);

		if (n < 0) {
			log_error("failed to read data from client: %s",
					strerror(errno));
			return;
		} else if (n == 0) {
			break;
		}
		log_info("Client requests received. Command=%d, Length=%d, Flags=%d", req.Command, req.Length, req.Flag);

		if (req.Command != CMD_TEST) {
			send_error(client, "Bad command!", 12);
			return ;
		}

		AC_ALPHABET_t text[req.Length+1];
		text[req.Length] = '\0';

		// TODO: read ...
		log_info("trying to read %d bytes", req.Length);
		n = read(client, text, req.Length);
		log_info("Client sent: %s", text);

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
