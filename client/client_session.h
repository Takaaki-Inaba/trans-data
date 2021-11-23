#ifndef _CLIENT_SESSION_H_
#define _CLIENT_SESSION_H_

typedef struct client_session client_session_t;

extern client_session_t *create_session(int connected_socket);
extern int begin_session(client_session_t *session, const char *send_file_path);
extern int put_session(client_session_t *session);
extern void destruct_session(client_session_t *session);
extern char *strerror_session(client_session_t *session);

#endif

