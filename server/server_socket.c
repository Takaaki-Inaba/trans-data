#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "../common/debug.h"

/** リスニングソケットを作成 */
int open_server_socket(const char *port)
{
	int sock = -1, optval, err = -1;
	struct addrinfo hints, *result = NULL, *rp = NULL;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;

	if ((err = getaddrinfo(NULL, port, &hints, &result)) != 0) {
		debug_print("getnameinfo %s", gai_strerror(err));
		goto end;
	}

	optval = 1;
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sock == -1) {
			continue;
		}

		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
			debug_perror("setsockopt");
			goto end;
		}

		if (bind(sock, rp->ai_addr, rp->ai_addrlen) == 0) {
			break;
		}
	}

	if (rp == NULL) {
		debug_print("Could not bind socket to any address");
		goto end;
	}

	if (listen(sock, SOMAXCONN) == -1) {
		debug_perror("listen");
		goto end;
	}

	err = 0;
end:
	if (result != NULL) {
		freeaddrinfo(result);
	}

	if (err != 0 && sock != -1) {
		close(sock);
	}

	return sock;
}

int close_server_socket(int server_socket)
{
	return close(server_socket);
}

