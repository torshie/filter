#include <stdlib.h>

#include "list.h"

void list_destroy(List* list) {
	while (list != NULL) {
		List* t = list->next;
		free(list);
		list = t;
	}
}
