#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <stdint.h>

/* ファイル名の最大サイズ（\0含む）
 * ファイル名を使ってシステムファイルを作成するときを
 * 考慮してNAME_MAXより小さな値を採用 */
#define FILE_NAME_MAX 200
struct message_F {
	/* F */
	char message_type;

	/* 送信ファイルサイズ */
	uint64_t file_size;

	/* ファイル名. \0終端 */
	char filename[FILE_NAME_MAX];
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

#define MESSAGE_TYPE_F 'F'
#define MESSAGE_TYPE_A 'A'
#define MESSAGE_TYPE_E 'E'

#define MESSAGE_TYPE_F_LENGTH sizeof(char) + sizeof(uint64_t) + FILE_NAME_MAX
#define MESSAGE_TYPE_A_LENGTH sizeof(char)
#define MESSAGE_TYPE_E_LENGTH sizeof(char) + ERROR_MESSAGE_LENGTH

#endif
