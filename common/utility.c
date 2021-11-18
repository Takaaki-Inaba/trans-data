#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "debug.h"

int set_recv_timeout(int sock, int timeout_sec)
{
	struct timeval recv_timeout;

	recv_timeout.tv_sec = (time_t)timeout_sec;
	recv_timeout.tv_usec = 0;
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout)) == -1) {
		debug_perror("setsockopt");
		return -1;
	}
	return 0;
}

int set_break_connection(int sock)
{
	struct linger l;

	l.l_onoff = 1;
	l.l_linger = 0;

	if (setsockopt(sock, SOL_SOCKET, SO_LINGER, &l, sizeof(l)) == -1) {
		debug_perror("setsockopt");
		return -1;
	}
	return 0;
}
