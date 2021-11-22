#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#include <unistd.h>
#include <limits.h>

#include "client_main.h"
#include "client_socket.h"
#include "communicate_server.h"
#include "../common/debug.h"
#include "../common/constant.h"

struct command_line_option {
#define PORT_NUMBER_MAX_LEN 5
	char port[PORT_NUMBER_MAX_LEN + 1];
	char hostname[HOST_NAME_MAX + 1];
	char send_file_path[PATH_MAX];
	int enable_debug;
};

static int initialize_client(struct command_line_option *option)
{
	if (option->enable_debug) {
		char log_filename[FILENAME_MAX];
		snprintf(log_filename, sizeof(log_filename), "/dev/stderr");
		if (debug_initialize(log_filename)) {
			goto error;
		}
		atexit(debug_finalize);
	}

	signal(SIGPIPE, SIG_IGN);

	return 0;

error:
	return -1;
}

static int validate_option(struct command_line_option *option)
{
	if (option->port[0] == '\0') {
		return -1;
	}
	if (option->hostname[0] == '\0') {
		return -1;
	}
	if (option->send_file_path[0] == '\0') {
		return -1;
	}
	return 0;
}

static void print_usage(void)
{
	fprintf(stderr, "trans-data-client -h hostname -p port -f filename [-d]\n");
}

/** 入力パラメータの解析 */
static int parse_option(int argc, char *argv[], struct command_line_option *option)
{
	int opt, ret = -1;

	opterr = 0;
	while ((opt = getopt(argc, argv, "h:p:f:d")) != -1) {
		switch (opt) {
		case 'h':
			snprintf(option->hostname,
				sizeof(option->hostname), "%s", optarg);
			break;
		case 'p':
			snprintf(option->port, sizeof(option->port), "%s", optarg);
			break;
		case 'f':
			snprintf(option->send_file_path,
				sizeof(option->send_file_path), "%s", optarg);
			break;
		case 'd':
			option->enable_debug = 1;
			break;
		default:
			goto end;
			break;
		}
	}

	if (optind < argc) {
		goto end;
	}

	ret = 0;
end:
	return ret;
}

int client_main(int argc, char *argv[])
{
	struct command_line_option option = {{ 0 }};
	int sock = -1;
	int ret_code = 1;

	if (parse_option(argc, argv, &option) || validate_option(&option)) {
		print_usage();
		goto end;
	}

	if (initialize_client(&option)) {
		goto end;
	}

	if ((sock = open_client_socket(option.hostname, option.port, SOCKET_TIMEOUT_SEC)) == -1) {
		goto end;
	}

	if (start_communicate_server(sock, option.send_file_path)) {
		goto end;
	}

	ret_code = 0;
end:
	if (sock != -1) {
		close_client_socket(sock);
	}
	return ret_code;
}

