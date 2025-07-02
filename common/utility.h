#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <stddef.h>

extern int set_socket_timeout(int sock, int timeout_sec);
extern int prepare_break_connection(int sock);
extern ssize_t send_all(int sock, const void *buf, size_t len, int flags);

#endif
