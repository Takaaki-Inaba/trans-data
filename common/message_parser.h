#ifndef _MESSAGE_PARSER_H_
#define _MESSAGE_PARSER_H_

#include "message.h"

extern int recv_message_F(int sock, struct message_F *msg_buf);
extern int recv_message_A(int sock, struct message_A *msg_buf);
extern int recv_message_E(int sock, struct message_E *msg_buf);

extern int send_message_F(int sock, const char *file_path);
extern int send_message_A(int sock);
extern int send_message_E(int sock, const char *error_message);

#endif
