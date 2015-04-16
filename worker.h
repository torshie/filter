#ifndef FILTER_WOKER_H_INCLUDED_
#define FILTER_WOKER_H_INCLUDED_

#include "pool.h"
#include "engine.h"

typedef struct WorkerContext {
	Engine* newer;
	Engine* current;
	int client_pipe[2];
	int waiting_count;
	int served_count;
	MemoryPool* pool;
} WorkerContext;

void worker_context_init(WorkerContext* context);
void* match_thread(void* arg);

#endif /* FILTER_WOKER_H_INCLUDED_ */
