#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "server_session.h"
#include "../common/debug.h"
#include "../common/message_lib.h"
#include "../common/utility.h"

typedef struct server_session {
	int connected_socket;
	int output_fd;
	uint64_t send_file_size;
	char *filename;
} server_session_t;

static int terminate_session(int connected_socket, const char *message)
{
	char tmp[256], error_message[ERROR_MESSAGE_LENGTH];
	char dummy;

	strerror_r(errno, tmp, sizeof(tmp));
	snprintf(error_message, sizeof(error_message), "%s: %s", message, tmp);

	if (send_message_E(connected_socket, error_message)) {
		debug_print("send_message_E error");
		goto error;
	}

	/* clientからのcloseを待つ */
	if (recv(connected_socket, &dummy, sizeof(dummy), 0) == -1) {
		debug_print("EOF read error");
		goto error;
	}

	return 0;

error:
	prepare_break_connection(connected_socket);
	return -1;
}

static int send_acknowledge(server_session_t *session)
{
	if (send_message_A(session->connected_socket) == -1) {
		debug_print("send_message_A error");
		goto error;
	}

	return 0;

error:
	prepare_break_connection(session->connected_socket);
	return -1;
}

static int recv_request(server_session_t *session)
{
	struct message_F msg_F = { 0 };

	if (recv_message_F(session->connected_socket, &msg_F)) {
		debug_print("recv_message_F error");
		goto error;
	}

	session->send_file_size = msg_F.file_size;
	if ((session->filename = strdup(msg_F.filename)) == NULL) {
		debug_perror("strdup");
		goto error;
	}

	return 0;

error:
	prepare_break_connection(session->connected_socket);
	return -1;
}

static int open_output_file(server_session_t *session)
{
	if ((session->output_fd = open(session->filename, O_WRONLY | O_CREAT, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) == -1) {
		debug_perror("open");
		goto error;
	}

	if (lockf(session->output_fd, F_TLOCK, 0) == -1) {
		debug_perror("lockf");
		goto error;
	}

	return 0;

error:
	terminate_session(session->connected_socket, "open_output_file");
	return -1;
}

static int close_output_file(server_session_t *session)
{
	if (lockf(session->output_fd, F_ULOCK, 0) == -1) {
		debug_perror("lockf");
	}

	close(session->output_fd);
	return 0;
}

server_session_t *create_session(int connected_socket)
{
	server_session_t *session;

	if ((session = calloc(1, sizeof(server_session_t))) == NULL) {
		debug_perror("calloc");
		return NULL;
	}
	session->connected_socket = connected_socket;
	return session;
}

int begin_session(server_session_t *session)
{
	if (recv_request(session)) {
		debug_print("recv_request");
		goto error;
	}

	if (open_output_file(session)) {
		debug_print("open_output_file error");
		goto error;
	}

	if (send_acknowledge(session)) {
		debug_print("send_acknowledge error");
		goto error;
	}

	return 0;

error:
	return -1;

}

int recv_file_session(server_session_t *session)
{
	char buf[BUFSIZ];
	ssize_t recv_size, n;
	uint64_t total_recv_size = 0;
	int ret = -1;

	while ((recv_size = recv(session->connected_socket, buf, sizeof(buf), 0)) > 0) {
		if ((n = write(session->output_fd, buf, (size_t)recv_size)) == -1) {
			debug_perror("write");
			terminate_session(session->connected_socket, "output file write error");
			goto end;
		}
		total_recv_size += (uint64_t)recv_size;
	}
	if (recv_size == -1) {
		debug_perror("recv");
		prepare_break_connection(session->connected_socket);
		goto end;
	}
	if (total_recv_size != session->send_file_size) {
		debug_print("send file size %lld but receive file size %lld", session->send_file_size, total_recv_size);
		terminate_session(session->connected_socket, "send file size not equal receive data size");
		goto end;
	}

	if (send_acknowledge(session)) {
		debug_print("send_acknowledge error");
		goto end;
	}

	ret = 0;
end:
	close_output_file(session);
	return ret;
}

void destruct_session(server_session_t *session)
{
	free(session->filename);
	free(session);
}
