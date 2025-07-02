#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "server_session.h"
#include "../common/debug.h"
#include "../common/message_lib.h"
#include "../common/utility.h"
#include "../common/constant.h"

/** クライアントと通信するスレッドのメイン */
static void *communicate_client(void *arg)
{
	int connected_socket = *(int *)arg;
	server_session_t *session = NULL;

	pthread_detach(pthread_self());

	if (set_socket_timeout(connected_socket, SOCKET_TIMEOUT_SEC)) {
		debug_print("set_socket_timeout error");
		goto end;
	}

	if ((session = create_session(connected_socket)) == NULL) {
		debug_print("create_session");
		goto end;
	}

	if (begin_session(session)) {
		debug_print("begin_session");
		goto end;
	}

	if (put_session(session)) {
		debug_print("put_session");
		goto end;
	}

end:
	if (session) {
		destruct_session(session);
	}
	close(connected_socket);
	return NULL;
}

int start_communicate_client(int connected_socket)
{
	int pthread_err;
	pthread_t dummy;

	if ((pthread_err = pthread_create(&dummy, NULL, communicate_client, &connected_socket)) != 0) {
		char tmp[256];
		strerror_r(pthread_err, tmp, sizeof(tmp));
		debug_print("pthread_create error: %s", tmp);
		return -1;
	}
	return 0;
}

