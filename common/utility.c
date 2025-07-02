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

ssize_t send_all(int sock, const void *buf, size_t len, int flags)
{
	size_t total_sent = 0;
	const char *p = (const char *)buf;

	while (total_sent < len) {
		ssize_t sent;

		if ((sent = send(sock, p + total_sent, len - total_sent, flags)) < 0) {
			return -1;
		}
		total_sent += (size_t)sent;
	}

	return (ssize_t)total_sent;
}

