#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <getopt.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include "../common/debug.h"
#include "server_main.h"
#include "server_socket.h"
#include "communicate_client.h"

struct command_line_option {
#define PORT_NUMBER_MAX_LEN 5
	char port[PORT_NUMBER_MAX_LEN + 1];
	char files_store_dir_path[PATH_MAX];
	int enable_debug;
};

static int initialize_server(struct command_line_option *option)
{
	if (daemon(1, 0) == -1) {
		debug_perror("daemon");
		goto error;
	}

	if (option->enable_debug) {
		char log_filename[NAME_MAX];
		snprintf(log_filename, sizeof(log_filename), "trans-data-server.%d", getpid());
		if (debug_initialize(log_filename)) {
			goto error;
		}
		atexit(debug_finalize);
	}

	signal(SIGPIPE, SIG_IGN);

	if (option->files_store_dir_path[0] != '\0') {
		if (chdir(option->files_store_dir_path) == -1) {
			debug_perror("chdir");
			goto error;
		}
	}
	return 0;

error:
	return -1;
}

static int validate_option(struct command_line_option *option)
{
	if (option->port[0] == '\0') {
		return -1;
	}
	return 0;
}

static void print_usage(void)
{
	fprintf(stderr, "trans-data-server -p port [-s files-store-directory] [-d]\n");
}

static int parse_option(int argc, char *argv[], struct command_line_option *option)
{
	int opt, ret = -1;

	opterr = 0;
	while ((opt = getopt(argc, argv, "p:s:d")) != -1) {
		switch (opt) {
		case 'p':
			snprintf(option->port, sizeof(option->port), "%s", optarg);
			break;
		case 's':
			snprintf(option->files_store_dir_path,
				sizeof(option->files_store_dir_path), "%s", optarg);
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

int server_main(int argc, char *argv[])
{
	struct command_line_option option = {{ 0 }};
	int server_socket = -1;

	if (parse_option(argc, argv, &option) || validate_option(&option)) {
		print_usage();
		goto end;
	}

	if (initialize_server(&option)) {
		goto end;
	}

	if ((server_socket = open_server_socket(option.port)) == -1) {
		goto end;
	}

	for (;;) {
		int connected_socket;

		if ((connected_socket = accept(server_socket, NULL, NULL)) == -1) {
			debug_perror("accept");
		}
		if (start_communicate_client(connected_socket)) {
			continue;
		}
	}

end:
	if (server_socket != -1) {
		close_server_socket(server_socket);
	}
	return 0;
}

