#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>

#include "cmdline.h"
#include "global.h"
#include "listen.h"
#include "log.h"
#include "worker.h"

static int load_chunk(Engine* engine, AC_ALPHABET_t* chunk,
		int length) {
	int begin = 0;
	while (begin < length) {
		int end = begin;
		while (end < length && chunk[end] != '\n') {
			++end;
		}
		if (end == length) {
			break;
		}
		engine_add_pattern(engine, chunk + begin, end - begin);
		begin = end + 1;
	}
	if (begin < length) {
		memmove(chunk, chunk + begin, (length - begin) * sizeof(*chunk));
	}
	return length - begin;
}

static void load_pattern_db(Engine* engine, int file) {
	AC_ALPHABET_t buffer[1024];
	while (1) {
		int n = read(file, buffer, sizeof(buffer));
		if (n == 0) {
			break;
		}
		load_chunk(engine, buffer, n / sizeof(*buffer));
	}
	ac_automata_finalize(engine->automata);
}

static void update_pattern_db(WorkerContext* context, int file) {
	context->newer = engine_init(context->pool);
	load_pattern_db(context->newer, file);
	context->newer->serial = context->current->serial + 1;
}

static int last_signal = 0;
static void store_signum(int s) {
	last_signal = s;
}

int main(int argc, char** argv) {
	if (!parse_cmdline(argc, argv)) {
		return 1;
	}
	signal(SIGUSR1, &store_signum);
	signal(SIGINT, &store_signum);
	global.worker =
			malloc(sizeof(WorkerContext) * global.config.thread_count);
	for (int i = 0; i < global.config.thread_count; ++i) {
		worker_context_init(global.worker + i);
		int fd = open(global.config.pattern_db, O_RDONLY);
		load_pattern_db(global.worker[i].newer, fd);
		close(fd);
	}

	pthread_t tid;
	pthread_create(&tid, NULL, &listen_thread, NULL);
	for (int i = 0; i < global.config.thread_count; ++i) {
		pthread_create(&tid, NULL, &match_thread, global.worker + i);
	}

	while (1) {
		pause();
		if (last_signal == SIGINT) {
			log_warning("Got SIGINT, shutting down.");
			break;
		}
		log_info("Got SIGUSR1, reloading pattern db.");
		for (int i = 0; i < global.config.thread_count; ++i) {
			int fd = open(global.config.pattern_db, O_RDONLY);
			update_pattern_db(global.worker + i, fd);
			close(fd);
		}
		log_info("done.");
	}
	return 0;
}
