#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#include "cmdline.h"
#include "log.h"
#include "list.h"
#include "global.h"

int parse_cmdline(int argc, char** argv) {
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "--listen") == 0) {
			List* l = (List*)malloc(sizeof(List) + sizeof(char*));
			l->next = global.config.listen_address;
			if (argv[i + 1] == NULL || argv[i + 1][0] == '-') {
				log_error("option --listen requires an argument.");
				return 0;
			}
			*FLT_LIST_GET(l, char*) = argv[i + 1];
			global.config.listen_address = l;
			++i;
		} else if (strcmp(argv[i], "--thread") == 0) {
			int s;
			if (argv[i + 1] == NULL || sscanf(argv[i + 1], "%d", &s) != 1
					|| s <= 0) {
				log_error("invalid thread number: %s", argv[i + 1]);
				return 0;
			}
			global.config.thread_count = s;
			++i;
		} else if (strcmp(argv[i], "--pattern") == 0) {
			if (argv[i + 1] == NULL || argv[i + 1][0] == '-') {
				log_error("option --pattern requires an argument.");
				return 0;
			}
			global.config.pattern_db = argv[i + 1];
			++i;
		}else {
			log_error("unknown command line string: %s", argv[i]);
			return 0;
		}
	}
	if (global.config.listen_address == NULL) {
		log_error("at least one --listen is required");
		return 0;
	}
	if (global.config.pattern_db == NULL) {
		log_error("option --pattern is required.");
		return 0;
	}
	if (access(global.config.pattern_db, F_OK | R_OK) != 0) {
		log_error("unable to access pattern database %s %s",
				global.config.pattern_db, strerror(errno));
		return 0;
	}
	if (global.config.thread_count == 0) {
		log_info("--thread missing, default to 10");
		global.config.thread_count = 10;
	}
	return 1;
}
