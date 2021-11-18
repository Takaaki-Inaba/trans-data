#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "../common/debug.h"
#include "../common/message_parser.h"
#include "../common/utility.h"
#include "../common/constant.h"

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
	set_break_connection(connected_socket);
	return -1;
}

static int reply_acknowledge(int connected_socket)
{
	if (send_message_A(connected_socket) == -1) {
		debug_print("recv_message_A error");
		goto error;
	}

	return 0;

error:
	set_break_connection(connected_socket);
	return -1;
}

static int initialize_session(int connected_socket, char *filename, uint64_t *file_size)
{
	struct message_F msg_F = { 0 };

	if (recv_message_F(connected_socket, &msg_F)) {
		debug_print("recv_message_F error");
		goto error;
	}

	strcpy(filename, msg_F.filename);
	*file_size = msg_F.file_size;

	return 0;

error:
	set_break_connection(connected_socket);
	return -1;
}

static int open_output_file(const char *filename)
{
	int fd;

	if ((fd = open(filename, O_WRONLY | O_CREAT, S_IRWXU | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) == -1) {
		debug_perror("open");
		goto error;
	}

	if (lockf(fd, F_TLOCK, 0) == -1) {
		debug_perror("lockf");
		goto error;
	}

	return 0;

error:
	return -1;
}

static void *communicate_client(void *arg)
{
	int connected_socket = *(int *)arg;
	char filename[NAME_MAX];
	uint64_t file_size;

	pthread_detach(pthread_self());

	if (set_recv_timeout(connected_socket, RECV_TIMEOUT_SEC)) {
		debug_print("set_recv_timeout error");
		goto end;
	}

	if (initialize_session(connected_socket, filename, &file_size)) {
		debug_print("initialize_session error");
		goto end;
	}

	if (open_output_file(filename)) {
		terminate_session(connected_socket, "open_output_file");
		debug_print("open_output_file error");
		goto end;
	}

	if (reply_acknowledge(connected_socket)) {
		debug_print("reply_acknowledge error");
		goto end;
	}


	// D受信
	// close

end:
	close(connected_socket);
	return NULL;
}

int start_communicate_client(int connected_socket)
{
	int pthread_err;
	pthread_t dummy;

	if ((pthread_err = pthread_create(&dummy, NULL, communicate_client, &connected_socket)) != 0) {
		char tmp[256];
		strerror_r(errno, tmp, sizeof(tmp));
		debug_print("pthread_create error: %s", tmp);
		return -1;
	}
	return 0;
}


