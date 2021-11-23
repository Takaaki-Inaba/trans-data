#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "debug.h"

int set_socket_timeout(int sock, int timeout_sec)
{
	struct timeval timeout;

	timeout.tv_sec = (time_t)timeout_sec;
	timeout.tv_usec = 0;
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
		debug_perror("setsockopt");
		return -1;
	}

	if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == -1) {
		debug_perror("setsockopt");
		return -1;
	}
	return 0;
}

/** ソケットの強制切断準備.
 * タイムアウトが発生するなど処理の継続が難しい場合は、peerにRSTパケットを送信することで
 * ソケットを強制切断する */
int prepare_break_connection(int sock)
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
