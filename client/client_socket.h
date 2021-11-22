#ifndef _CLIENT_SOCKET_H_
#define _CLIENT_SOCKET_H_

extern int open_client_socket(const char *hostname, const char *port_no, int timeout_sec);
extern int close_client_socket(int connected_socket);

#endif
