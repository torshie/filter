#ifndef FILTER_ENGINE_H_INCLUDED_
#define FILTER_ENGINE_H_INCLUDED_

#include "smm/ahocorasick.h"
#include "pool.h"

typedef struct Engine {
	AC_AUTOMATA_t* automata;
	int fresh;
	List** tail;
	List* match;
	MemoryPool* pool;
	int serial;
} Engine;

Engine* engine_init(MemoryPool* pool);
void engine_destroy(Engine* engine);
void engine_increase(Engine* engine);
void engine_decrease(Engine* engine);
void engine_add_pattern(Engine* engine, const AC_ALPHABET_t* pattern,
		int length);
void engine_feed_text(Engine* engine, const AC_ALPHABET_t* text,
		int length);
List* engine_get_result(Engine* engine);

#endif /* FILTER_ENGINE_H_INCLUDED_ */
