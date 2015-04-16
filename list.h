#ifndef FILTER_LIST_H_INCLUDED_
#define FILTER_LIST_H_INCLUDED_

typedef struct List {
	struct List* next;
	char data[0];
} List;

#define FLT_LIST_GET(l, type) \
	((type*)((l)->data))

void list_destroy(List* list);

#endif /* FILTER_LIST_H_INCLUDED_ */
