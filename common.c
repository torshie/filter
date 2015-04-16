#include <string.h>

#include "common.h"

int int_to_dec(int value, char* str) {
	if (value == 0) {
		str[0] = '0';
		return 1;
	}

	int negative = (value < 0);
	if (negative) {
		value = -value;
	}
	char buffer[32];
	int offset = sizeof(buffer) - 1;
	while (value > 0) {
		buffer[offset] = value % 10 + '0';
		value /= 10;
		--offset;
	}
	if (negative) {
		buffer[offset--] = '-';
	}
	int size = sizeof(buffer) - offset - 1;
	memcpy(str, buffer + offset + 1, size);
	return size;
}
