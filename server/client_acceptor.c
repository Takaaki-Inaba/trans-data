#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "client_acceptor.h"
#include "../common/debug.h"

typedef struct client_acceptor_private_t {
	int listening_socket;
} CLIENT_ACCEPTOR_PRIVATE;

static int initialize_server_socket(const char *port)
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
			debug_print("setsockopt %s", strerror(errno));
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
		debug_print("listen %s", strerror(errno));
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


static void s_destruct(struct client_acceptor *self)
{
	free(self->private_data);
	free(self);
}

static int s_open_socket(struct client_acceptor *self, const char *port)
{
	if ((self->private_data->listening_socket = initialize_server_socket(port)) == -1) {
		return -1;
	}
	return 0;
}

static int s_accept(struct client_acceptor *self)
{
	int client_socket;

	if ((client_socket = accept(self->private_data->listening_socket, NULL, NULL)) == -1) {
		debug_print("accept %s", strerror(errno));
	}
	return client_socket;
}

int create_client_acceptor(struct client_acceptor **ca)
{
	struct client_acceptor *tca = NULL;
	CLIENT_ACCEPTOR_PRIVATE *tp = NULL;

	if ((tca = calloc(1, sizeof(struct client_acceptor))) == NULL) {
		goto error;
	}
	if ((tp = calloc(1, sizeof(CLIENT_ACCEPTOR_PRIVATE))) == NULL) {
		goto error;
	}

	tca->private_data = tp;

	*ca = tca;
	return 0;

error:
	if (tp) {
		free(tp);
	}
	if (tca) {
		free(tca);
	}
	return -1;
}
