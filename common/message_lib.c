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
	uint64_t fsize;
	char *buf = NULL, *p;
	int ret = -1;

	if ((buf = calloc(1, MESSAGE_TYPE_F_LENGTH)) == NULL) {
		debug_perror("calloc");
		goto end;
	}

	if (recv(sock, buf, MESSAGE_TYPE_F_LENGTH, MSG_WAITALL) == -1) {
		debug_perror("recv");
		goto end;
	}

	p = buf;
	msg_buf->message_type = *p;
	p += sizeof(msg_buf->message_type);
	if (msg_buf->message_type != MESSAGE_TYPE_F) {
		goto end;
	}
	memcpy(&fsize, p, sizeof(fsize));
	p += sizeof(fsize);
	msg_buf->file_size = be64toh(fsize);
	memcpy(msg_buf->filename, p, sizeof(msg_buf->filename));

	ret = 0;
end:
	if (buf) {
		free(buf);
	}
	return ret;
}

int recv_message_A(int sock, struct message_A *msg_buf)
{
	char *buf = NULL, *p;
	int ret = -1;

	if ((buf = calloc(1, MESSAGE_TYPE_A_LENGTH)) == NULL) {
		debug_perror("calloc");
		goto end;
	}

	if (recv(sock, buf, MESSAGE_TYPE_A_LENGTH, MSG_WAITALL) == -1) {
		debug_perror("recv");
		goto end;
	}

	p = buf;
	msg_buf->message_type = *p;
	if (msg_buf->message_type != MESSAGE_TYPE_A) {
		goto end;
	}

	ret = 0;
end:
	if (buf) {
		free(buf);
	}
	return ret;
}

int recv_message_E(int sock, struct message_E *msg_buf)
{
	char *buf = NULL, *p;
	int ret = -1;

	if ((buf = calloc(1, MESSAGE_TYPE_E_LENGTH)) == NULL) {
		debug_perror("calloc");
		goto end;
	}

	if (recv(sock, buf, MESSAGE_TYPE_E_LENGTH, MSG_WAITALL) == -1) {
		debug_perror("recv");
		goto end;
	}

	p = buf;
	msg_buf->message_type = *p;
	p += sizeof(msg_buf->message_type);
	if (msg_buf->message_type != MESSAGE_TYPE_E) {
		goto end;
	}
	memcpy(msg_buf->error_message, p, ERROR_MESSAGE_LENGTH);

	ret = 0;
end:
	if (buf) {
		free(buf);
	}
	return ret;
}

int send_message_F(int sock, const char *filename, uint64_t file_size)
{
	uint64_t fsize;
	char *buf = NULL, *p;
	int ret = -1;

	if ((buf = calloc(1, MESSAGE_TYPE_F_LENGTH)) == NULL) {
		debug_perror("calloc");
		goto end;
	}

	p = buf;
	*p = MESSAGE_TYPE_F;
	p += sizeof(char);

	fsize = htobe64(file_size);
	memcpy(p, &fsize, sizeof(uint64_t));
	p += sizeof(uint64_t);

	memcpy(p, filename, FILE_NAME_MAX);

	if (send(sock, buf, MESSAGE_TYPE_F_LENGTH, 0) == -1) {
		debug_perror("send");
		goto end;
	}

	ret = 0;
end:
	if (buf) {
		free(buf);
	}
	return ret;
}

int send_message_A(int sock)
{
	char *buf = NULL, *p;
	int ret = -1;

	if ((buf = calloc(1, MESSAGE_TYPE_A_LENGTH)) == NULL) {
		debug_perror("calloc");
		goto end;
	}

	p = buf;
	*p = MESSAGE_TYPE_A;

	if (send(sock, buf, MESSAGE_TYPE_A_LENGTH, 0) == -1) {
		debug_perror("send");
		goto end;
	}

	ret = 0;
end:
	if (buf) {
		free(buf);
	}
	return ret;
}

int send_message_E(int sock, const char *error_message)
{
	char *buf = NULL, *p;
	int ret = -1;

	if ((buf = calloc(1, MESSAGE_TYPE_E_LENGTH)) == NULL) {
		debug_perror("calloc");
		goto end;
	}

	p = buf;
	*p = MESSAGE_TYPE_E;
	p += sizeof(char);

	snprintf(p, ERROR_MESSAGE_LENGTH, "%s", error_message);

	if (send(sock, buf, MESSAGE_TYPE_E_LENGTH, 0) == -1) {
		debug_perror("send");
		goto end;
	}

	ret = 0;
end:
	if (buf) {
		free(buf);
	}
	return ret;
}
