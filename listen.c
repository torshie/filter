#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>
#include <unistd.h>
#include "listen.h"
#include "list.h"
#include "log.h"
#include "worker.h"
#include "global.h"

typedef struct {
	int size;
	struct pollfd fd[1];
} AddressInfo;

static int parse_inet_address(const char* s, struct sockaddr_in* address) {
	int a, b, c, d, port;
	if (sscanf(s, "%d.%d.%d.%d:%d", &a, &b, &c, &d, &port) != 5
			|| a < 0 || b < 0 || c < 0 || d < 0
			|| a > 255 || b > 255 || c > 255 || d > 255
			|| port <= 0 || port >= (1 << 16) - 1) {
		return 0;
	}
	memset(address, 0, sizeof(*address));
	address->sin_family = AF_INET;
	address->sin_port = htons(port);
	address->sin_addr.s_addr = htonl((a << 24) | (b << 16) | (c << 8) | d);
	return 1;
}

static int get_listen_count() {
	List* l = global.config.listen_address;
	int n = 0;
	while (l != NULL) {
		++n;
		l = l->next;
	}
	return n;
}

static void fill_un_addr(struct sockaddr_un* un, const char* path) {
	memset(un, 0, sizeof(*un));
	un->sun_family = AF_UNIX;
	strcpy(un->sun_path, path);
	if (un->sun_path[0] == '@') {
		un->sun_path[0] = '\0';
	}
}

static int listen_address(const char* str) {
	union {
		struct sockaddr_in in;
		struct sockaddr_un un;
	} addr;
	int fd;
	socklen_t length;
	if (!parse_inet_address(str, &addr.in)) {
		fill_un_addr(&addr.un, str);
		fd = socket(AF_UNIX, SOCK_STREAM, 0);
		length = sizeof(addr.un);
		if (addr.un.sun_path[0] != '\0') {
			unlink(addr.un.sun_path);
		}
	} else {
		length = sizeof(addr.in);
		fd = socket(AF_INET, SOCK_STREAM, 0);
		int one = 1;
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	}
	if (bind(fd, (struct sockaddr*)&addr, length) != 0) {
		log_error("failed to bind address %s, %s", str, strerror(errno));
		return -1;
	}
	listen(fd, 10);
	return fd;
}

static AddressInfo* parse_address_info() {
	int size = get_listen_count();
	AddressInfo* info = malloc(sizeof(AddressInfo)
			+ sizeof(struct pollfd) * (size - 1));
	info->size = size;
	List* l = global.config.listen_address;
	for (int i = 0; i < size; ++i) {
		info->fd[i].fd = listen_address(*FLT_LIST_GET(l, char*));
		info->fd[i].events = POLLIN;
		info->fd[i].revents = 0;
		l = l->next;
	}
	return info;
}

static void dispatch_client(int client) {
	WorkerContext* context = global.worker;
	int offset = rand() % global.config.thread_count;
	int32_t min = INT32_MAX;
	int position = offset;
	for (int i = 0; i < global.config.thread_count; ++i, ++offset) {
		if (offset >= global.config.thread_count) {
			offset = 0;
		}
		int32_t waiting = context[offset].waiting_count
				- context[offset].served_count;
		if (waiting < min) {
			min = waiting;
			position = offset;
		}
	}
	write(context[position].client_pipe[1], &client, sizeof(client));
	++context[position].waiting_count;
}

void* listen_thread(void* p) {
	(void)p;

	AddressInfo* info = parse_address_info();
	while (1) {
		int r = poll(info->fd, info->size, -1);
		if (r < 0) {
			log_warning("poll failed %s", strerror(errno));
			continue;
		}
		for (int i = 0; i < info->size; ++i) {
			if (info->fd[i].revents != 0) {
				int client = accept(info->fd[i].fd, NULL, NULL);
				if (client < 0) {
					log_error("accept %d failed: %s", info->fd[i].fd,
							strerror(errno));
					continue;
				}
				dispatch_client(client);
			}
		}
	}

	return NULL;
}
