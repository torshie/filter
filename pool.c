#include <stddef.h>
#include <stdlib.h>

#include "pool.h"
#include "list.h"

enum {
	kBufferSize = 1024 * 1024 * 4 - sizeof(void*) - 8
};

typedef struct Chunk {
	int offset;
	int _padding_;
	char data[kBufferSize];
} Chunk;

MemoryPool* mpool_create() {
	List* node = malloc(sizeof(List) + sizeof(Chunk));
	node->next = NULL;
	FLT_LIST_GET(node, Chunk)->offset = 0;
	MemoryPool* pool = malloc(sizeof(MemoryPool));
	pool->available = node;
	pool->head = node;
	return pool;
}

void mpool_destroy(MemoryPool* pool) {
	list_destroy(pool->head);
	free(pool);
}

void mpool_reset(MemoryPool* pool) {
	List* node = pool->head;
	while (node != NULL) {
		List* t = node->next;
		FLT_LIST_GET(node, Chunk)->offset = 0;
		node = t;
	}
}

void* mpool_alloc(MemoryPool* pool, size_t size) {
	size = (size + 7) / 8 * 8;
	Chunk* ck = FLT_LIST_GET(pool->available, Chunk);
	if (ck->offset + size > kBufferSize) {
		if (pool->available->next == NULL) {
			pool->available->next = malloc(sizeof(List) + sizeof(Chunk));
			pool->available->next->next = NULL;
		}
		pool->available = pool->available->next;
		ck = FLT_LIST_GET(pool->available, Chunk);
	}
	void* r = ck->data + ck->offset;
	ck->offset += size;
	return r;
}
