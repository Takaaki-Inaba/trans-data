#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>

#include "../common/utility.h"
#include "../common/debug.h"
#include "../common/constant.h"

int open_client_socket(const char *hostname, const char *port_no, int timeout_sec)
{
	struct addrinfo hints, *res = NULL;
	int gai_err, err_state = -1;
	int sock = -1;

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((gai_err = getaddrinfo(hostname, port_no, &hints, &res)) != 0) {
		debug_print("getaddrinfo error %s", gai_strerror(gai_err));
		goto end;
	}

	if ((sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		debug_perror("socket");
		goto end;
	}

	/* ソケットタイムアウトをセット
	 * TODO: タイムアウト値は外部ファイルから読み込むように変更*/
	if (set_socket_timeout(sock, SOCKET_TIMEOUT_SEC)) {
		debug_print("set_socket_timeout error");
		goto end;
	}

	if (connect(sock, res->ai_addr, res->ai_addrlen) == -1) {
		debug_perror("connect");
		goto end;
	}

	err_state = 0;
end:
	if (res != NULL) {
		freeaddrinfo(res);
	}

	if (err_state == -1) {
		if (sock != -1) {
			close(sock);
			sock = -1;
		}
	}

	return sock;
}

int close_client_socket(int connected_socket)
{
	return close(connected_socket);
}
