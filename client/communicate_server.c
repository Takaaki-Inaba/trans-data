#include <unistd.h>

#include "client_session.h"
#include "../common/debug.h"

static int communicate_server(int connected_socket, const char *send_file_path)
{
	int ret = -1;
	client_session_t *session;

	if ((session = create_session(connected_socket)) == NULL) {
		goto end;
	}

	if (begin_session(session, send_file_path)) {
		debug_print("begin_session error: %s", strerror_session(session));
		goto end;
	}

	if (put_session(session)) {
		debug_print("put_session error: %s", strerror_session(session));
		goto end;
	}

	ret = 0;
end:
	if (session) {
		destruct_session(session);
	}
	return ret;
}


int start_communicate_server(int connected_socket, const char *send_file_path)
{
	return communicate_server(connected_socket, send_file_path);
}
