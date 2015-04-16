#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#include "log.h"

enum LogLevel {
	kDebug = 1, kInfo, kWarning, kError, kFatal
};

static size_t get_current_time(char* buf, size_t size) {
	time_t a = time(NULL);
	struct tm t;
	localtime_r(&a, &t);
	return strftime(buf, size, "[%F %T] ", &t);
}

static void write_log(FILE* dest, int level, const char* fmt,
		va_list ap) {
	const char* const kLevelName[] = {
			NULL, "DEBUG: ", "INFO: ", "WARN: ", "ERROR: ", "FATAL: "
	};
	char time_string[64];
	size_t size = get_current_time(time_string, sizeof(time_string));
	fwrite(time_string, 1, size, dest);
	fputs(kLevelName[level], dest);
	vfprintf(dest, fmt, ap);
	fputc('\n', dest);
}

#define FLT_LOG(level) \
	va_list ap; \
	va_start(ap, fmt); \
	write_log(stderr, level, fmt, ap); \
	va_end(ap)

void log_debug(const char* fmt, ...) {
	FLT_LOG(kDebug);
}

void log_info(const char* fmt, ...) {
	FLT_LOG(kInfo);
}

void log_warning(const char* fmt, ...) {
	FLT_LOG(kWarning);
}

void log_error(const char* fmt, ...) {
	FLT_LOG(kError);
}

void log_fatal(const char* fmt, ...) {
	FLT_LOG(kFatal);
	exit(1);
}

#undef FLT_LOG
