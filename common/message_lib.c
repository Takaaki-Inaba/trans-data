#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <endian.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "debug.h"
#include "message_lib.h"

int recv_message_F(int sock, struct message_F *msg_buf)
{
	uint64_t file_size_nb;

	if (recv(sock, &msg_buf->message_type, sizeof(msg_buf->message_type), MSG_WAITALL) == -1) {
		debug_perror("recv");
		goto error;
	}
	if (msg_buf->message_type != MESSAGE_TYPE_F) {
		goto error;
	}

	if (recv(sock, &file_size_nb, sizeof(file_size_nb), MSG_WAITALL) == -1) {
		debug_perror("recv");
		goto error;
	}
	msg_buf->file_size = be64toh(file_size_nb);

	if (recv(sock, msg_buf->filename, sizeof(msg_buf->filename), MSG_WAITALL) == -1) {
		debug_perror("recv");
		goto error;
	}
	return 0;

error:
	return -1;
}

int recv_message_A(int sock, struct message_A *msg_buf)
{
	if (recv(sock, &msg_buf->message_type, sizeof(msg_buf->message_type), MSG_WAITALL) == -1) {
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
	if (recv(sock, &msg_buf->message_type, sizeof(msg_buf->message_type), MSG_WAITALL) == -1) {
		debug_perror("recv");
		goto error;
	}
	if (msg_buf->message_type != MESSAGE_TYPE_E) {
		goto error;
	}
	if (recv(sock, msg_buf->error_message, sizeof(msg_buf->error_message), MSG_WAITALL) == -1) {
		debug_perror("recv");
		goto error;
	}
	return 0;

error:
	return -1;
}

int send_message_F(int sock, const char *filename, uint64_t file_size)
{
	struct message_F msg = { 0 };

	msg.message_type = MESSAGE_TYPE_F;
	msg.file_size = htobe64(file_size);
	snprintf(msg.filename, sizeof(msg.filename), "%s", filename);

	if (send(sock, &msg.message_type, sizeof(msg.message_type), 0) == -1) {
		debug_perror("send");
		goto error;
	}
	if (send(sock, &msg.file_size, sizeof(msg.file_size), 0) == -1) {
		debug_perror("send");
		goto error;
	}
	if (send(sock, &msg.filename, sizeof(msg.filename), 0) == -1) {
		debug_perror("send");
		goto error;
	}

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
	if (send(sock, &msg.message_type, sizeof(msg.message_type), 0) == -1) {
		debug_perror("send");
		return -1;
	}
	if (send(sock, &msg.error_message, sizeof(msg.error_message), 0) == -1) {
		debug_perror("send");
		return -1;
	}

	return 0;

}
