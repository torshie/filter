#ifndef FILTER_LOG_H_INCLUDED_
#define FILTER_LOG_H_INCLUDED_

void log_fatal(const char* fmt, ...) __attribute__((__noreturn__));
void log_error(const char* fmt, ...);
void log_warning(const char* fmt, ...);
void log_info(const char* fmt, ...);
void log_debug(const char* fmt, ...);

#endif /* FILTER_LOG_H_INCLUDED_ */
