#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <limits.h>
#include <stdint.h>

#define MESSAGE_TYPE_F 'F'
#define MESSAGE_TYPE_A 'A'
#define MESSAGE_TYPE_E 'E'

struct message_F {
	/* F */
	char message_type;

	/* 送信ファイルサイズ */
	uint64_t file_size;

	/* ファイル名. \0終端 */
	char filename[NAME_MAX];
};

struct message_A {
	/* A */
	char message_type;
};

#define ERROR_MESSAGE_LENGTH 1024
struct message_E {
	/* E */
	char message_type;

	/* エラーメッセージ. \0終端 */
	char error_message[ERROR_MESSAGE_LENGTH];
};

#endif
