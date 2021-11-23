#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <libgen.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "client_session.h"
#include "../common/debug.h"
#include "../common/message_lib.h"
#include "../common/utility.h"

enum response_type {
	RESPONSE_OK,
	RESPONSE_ERROR,
	RESPONSE_UNEXPECTED,
};

typedef struct client_session {
	int connected_socket;
	char *send_file_path;
	uint64_t send_file_size;
	char *response_error_message;
} client_session_t;

client_session_t *create_session(int connected_socket)
{
	client_session_t *session;

	if ((session = calloc(1, sizeof(client_session_t))) == NULL) {
		debug_perror("calloc");
		return NULL;
	}
	session->connected_socket = connected_socket;
	return session;
}

static int recv_acknowledge(client_session_t *session, enum response_type *resp)
{
	char message_type;

	if (recv(session->connected_socket, &message_type, sizeof(message_type), MSG_PEEK | MSG_WAITALL) == -1) {
		debug_perror("recv message type");
		goto error;
	}

	if (message_type == MESSAGE_TYPE_A) {
		struct message_A msg_A = { 0 };

		if (recv_message_A(session->connected_socket, &msg_A)) {
			debug_print("recv_message_A");
			goto error;
		}
		*resp = RESPONSE_OK;
	} else if (message_type == MESSAGE_TYPE_E) {
		struct message_E msg_E = { 0 };

		if (recv_message_E(session->connected_socket, &msg_E)) {
			debug_print("recv_message_E");
			goto error;
		}
		*resp = RESPONSE_ERROR;
		if ((session->response_error_message = strdup(msg_E.error_message)) == NULL) {
			debug_perror("strdup");
			goto error;
		}
	} else {
		*resp = RESPONSE_UNEXPECTED;
		debug_print("recv unexpected message type");
	}

	return 0;

error:
	return -1;
}

int begin_session(client_session_t *session, const char *send_file_path)
{
	enum response_type resp;
	struct stat sb;
	char *p = NULL;

	if ((session->send_file_path = strdup(send_file_path)) == NULL) {
		debug_perror("strdup");
		goto error;
	}

	if (stat(send_file_path, &sb) == -1) {
		debug_perror("stat");
		goto error;
	}
	session->send_file_size = (uint64_t)sb.st_size;

	p = strdup(session->send_file_path);
	if (send_message_F(session->connected_socket, basename(p), session->send_file_size)) {
		debug_print("recv_message_F error");
		goto error;
	}
	free(p);

	if (recv_acknowledge(session, &resp)) {
		debug_print("recv_acknowledge");
		goto error;
	}

	return (resp == RESPONSE_OK) ? 0 : -1;

error:
	if (p) {
		free(p);
	}
	prepare_break_connection(session->connected_socket);
	return -1;
}

int put_session(client_session_t *session)
{
	int fd = -1;
	enum response_type resp;
	char buf[BUFSIZ];
	ssize_t read_byte;

	if ((fd = open(session->send_file_path, O_RDONLY)) == -1) {
		debug_perror("open");
		goto error;
	}

	while ((read_byte = read(fd, buf, sizeof(buf))) > 0) {
		if (send(session->connected_socket, buf, (size_t)read_byte, 0) == -1) {
			debug_perror("send");
			goto error;
		}
	}
	if (read_byte == -1) {
		debug_perror("read");
		goto error;
	}

	if (shutdown(session->connected_socket, SHUT_WR) == -1) {
		debug_perror("shutdown");
		goto error;
	}

	if (recv_acknowledge(session, &resp)) {
		debug_print("recv_acknowledge");
		goto error;
	}

	close(fd);
	return (resp == RESPONSE_OK) ? 0 : -1;

error:
	if (fd != -1) {
		close(fd);
	}
	prepare_break_connection(session->connected_socket);
	return -1;
}

char *strerror_session(client_session_t *session)
{
	return session->response_error_message;
}

void destruct_session(client_session_t *session)
{
	if (session->send_file_path) {
		free(session->send_file_path);
	}
	if (session->response_error_message) {
		free(session->response_error_message);
	}
	free(session);
}

