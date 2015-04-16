#include <string.h>

#include "global.h"

GlobalStruct global;

void init_global() {
	memset(&global, 0, sizeof(global));
}
