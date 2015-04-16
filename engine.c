#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "list.h"
#include "log.h"
#include "engine.h"

Engine* engine_init(MemoryPool* pool) {
	Engine* engine = malloc(sizeof(Engine));
	engine->automata = ac_automata_init();
	engine->fresh = 1;
	engine->pool = pool;
	engine->match = NULL;
	engine->tail = &engine->match;
	engine->serial = 0;
	return engine;
}

void engine_destroy(Engine* engine) {
	ac_automata_release(engine->automata);
	free(engine);
}

void engine_add_pattern(Engine* engine, const AC_ALPHABET_t* pattern,
		int length) {
	AC_PATTERN_t ptn = {
			pattern, length,
			{NULL}
	};
	ac_automata_add(engine->automata, &ptn);
}

static int handle_match(AC_MATCH_t* match, void* p) {
	Engine* engine = p;
	List* node = mpool_alloc(engine->pool, sizeof(List) + sizeof(int) * 2);
	FLT_LIST_GET(node, int)[0] = match->position;
	FLT_LIST_GET(node, int)[1] = match->patterns[0].length;
	*engine->tail = node;
	engine->tail = &node->next;
	return 0;
}

void engine_feed_text(Engine* engine, const AC_ALPHABET_t* text,
		int length) {
	AC_TEXT_t t = {
			text, length
	};
	ac_automata_search(engine->automata, &t, !engine->fresh, &handle_match,
			engine);
	if (engine->fresh) {
		engine->fresh = 0;
	}
}

List* engine_get_result(Engine* engine) {
	*engine->tail = NULL;
	engine->fresh = 1;
	return engine->match;
}
