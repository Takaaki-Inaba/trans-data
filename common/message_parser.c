#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <endian.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "debug.h"
#include "message_parser.h"

int recv_message_F(int sock, struct message_F *msg_buf)
{
	uint64_t file_size_nb;

	if (recv(sock, &msg_buf->message_type, sizeof(msg_buf->message_type), 0) == -1) {
		debug_perror("recv");
		goto error;
	}
	if (msg_buf->message_type != MESSAGE_TYPE_F) {
		goto error;
	}

	if (recv(sock, &file_size_nb, sizeof(file_size_nb), 0) == -1) {
		debug_perror("recv");
		goto error;
	}
	msg_buf->file_size = be64toh(file_size_nb);

	if (recv(sock, msg_buf->filename, sizeof(msg_buf->filename), 0) == -1) {
		debug_perror("recv");
		goto error;
	}
	return 0;

error:
	return -1;
}

int recv_message_A(int sock, struct message_A *msg_buf)
{
	if (recv(sock, &msg_buf->message_type, sizeof(msg_buf->message_type), 0) == -1) {
		debug_perror("recv");
		goto error;
	}
	if (msg_buf->message_type != MESSAGE_TYPE_A) {
		goto error;
	}
	return 0;

error:
	return -1;
}

int recv_message_E(int sock, struct message_E *msg_buf)
{
	if (recv(sock, &msg_buf->message_type, sizeof(msg_buf->message_type), 0) == -1) {
		debug_perror("recv");
		goto error;
	}
	if (msg_buf->message_type != MESSAGE_TYPE_F) {
		goto error;
	}
	if (recv(sock, msg_buf->error_message, sizeof(msg_buf->error_message), 0) == -1) {
		debug_perror("recv");
		goto error;
	}
	return 0;

error:
	return -1;
}

int send_message_F(int sock, const char *file_path)
{
	struct message_F msg = { 0 };
	struct stat sb;
	char *p = NULL;

	msg.message_type = MESSAGE_TYPE_F;
	if (stat(file_path, &sb) == -1) {
		debug_perror("stat");
		goto error;
	}
	msg.file_size = htobe64(sb.st_size);
	p = strdup(file_path);
	snprintf(msg.filename, sizeof(msg.filename), "%s", basename(p));

	if (send(sock, &msg, sizeof(msg), 0) == -1) {
		debug_perror("send");
		free(p);
		goto error;
	}

	free(p);
	return 0;

error:
	return -1;
}

int send_message_A(int sock)
{
	struct message_A msg = { 0 };

	msg.message_type = MESSAGE_TYPE_A;
	if (send(sock, &msg, sizeof(msg), 0) == -1) {
		debug_perror("send");
		return -1;
	}

	return 0;

}

int send_message_E(int sock, const char *error_message)
{
	struct message_E msg = { 0 };

	msg.message_type = MESSAGE_TYPE_E;
	snprintf(msg.error_message, sizeof(msg.error_message), "%s", error_message);
	if (send(sock, &msg, sizeof(msg), 0) == -1) {
		debug_perror("send");
		return -1;
	}

	return 0;

}
