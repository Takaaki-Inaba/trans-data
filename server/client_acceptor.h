#ifndef _CLIENT_ACCEPTOR_H_
#define _CLIENT_ACCEPTOR_H_

typedef struct client_acceptor_private_t CLIENT_ACCEPTOR_PRIVATE;

struct client_acceptor {
	CLIENT_ACCEPTOR_PRIVATE *private_data;

	void (*destruct)(struct client_acceptor *self);
	int (*open_socket)(struct client_acceptor *self, const char *port);
	int (*accept)(struct client_acceptor *self);
};

int create_client_acceptor(struct client_acceptor **ca);

#endif
