#ifndef _SERVER_SESSION_H_
#define _SERVER_SESSION_H_

typedef struct server_session server_session_t;

extern server_session_t *create_session(int connected_socket);
extern int begin_session(server_session_t *session);
extern int recv_file_session(server_session_t *session);
extern void destruct_session(server_session_t *session);

#endif
