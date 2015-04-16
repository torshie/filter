#ifndef FILTER_GLOBAL_H_INCLUDED_
#define FILTER_GLOBAL_H_INCLUDED_

#include "list.h"
#include "worker.h"

typedef struct GlobalStruct {
	struct {
		List* listen_address;
		int thread_count;
		const char* pattern_db;
	} config;
	WorkerContext* worker;
} GlobalStruct;

extern GlobalStruct global;

void init_global();

#endif /* FILTER_GLOBAL_H_INCLUDED_ */
