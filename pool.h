#ifndef FILTER_POOL_H_INCLUDED_
#define FILTER_POOL_H_INCLUDED_

#include "list.h"

typedef struct MemoryPool {
	struct List* available;
	struct List* head;
} MemoryPool;

MemoryPool* mpool_create();
void mpool_destroy(MemoryPool* pool);
void mpool_reset(MemoryPool* pool);
void* mpool_alloc(MemoryPool* pool, size_t size);

#endif /* FILTER_POOL_H_INCLUDED_ */
