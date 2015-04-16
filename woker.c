#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>

#include "log.h"
#include "list.h"
#include "worker.h"
#include "engine.h"
#include "common.h"
#include "smm/actypes.h"

static Engine* get_match_engine(WorkerContext* context) {
	if (context->newer->serial > context->current->serial) {
		engine_destroy(context->current);
		context->current = context->newer;
	}
	return context->current;
}

static void send_result(int client, List* result) {
	char buffer[1024];
	buffer[0] = '0';
	buffer[1] = '\n';

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
		write(client, buffer, offset);
		offset = 0;
	} while (result != NULL);
	write(client, "\n", 1);
}

static void serve_client(int client, WorkerContext* context) {
	Engine* engine = get_match_engine(context);
	AC_ALPHABET_t text[1024];
	for (;;) {
		int n = read(client, text, sizeof(text));
		if (n < 0) {
			log_error("failed to read data from client: %s",
					strerror(errno));
			return;
		} else if (n == 0) {
			break;
		}
		engine_feed_text(engine, text, n / sizeof(text[0]));
	};

	List* result = engine_get_result(engine);
	send_result(client, result);
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
