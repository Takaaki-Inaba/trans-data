#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <getopt.h>

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include "../common/debug.h"
#include "server_main.h"
#include "client_acceptor.h"

struct trans_data_server_option {
#define PORT_NUMBER_MAX_LEN 5
	char port[PORT_NUMBER_MAX_LEN + 1];
	char files_store_dir_path[PATH_MAX];
	int enable_debug;
};

static int initialize_trans_data_server(struct trans_data_server_option *option)
{
	if (option->enable_debug) {
		char log_filename[FILENAME_MAX];
		snprintf(log_filename, sizeof(log_filename), "trans-data-server.%d", getpid());
		if (debug_initialize(log_filename)) {
			goto error;
		}
		atexit(debug_finalize);
	}

	/* closed socketへのwriteで発生するSIGPIPEを無視する
	 * TODO: その他のシグナルの扱いをどうするかを仕様含め検討 */
	signal(SIGPIPE, SIG_IGN);

	if (daemon(1, 0) == -1) {
		debug_print("daemon error: %s", strerror(errno));
		goto error;
	}

	if (chdir(option->files_store_dir_path) == -1) {
		debug_print("chdir error: %s", strerror(errno));
		goto error;
	}
	return 0;

error:
	return -1;
}

static int validate_option(struct trans_data_server_option *option)
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

/** 入力パラメータの解析 */
static int parse_option(int argc, char *argv[], struct trans_data_server_option *option)
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

	if (optind >= argc) {
		goto end;
	}

	ret = 0;
end:
	return ret;
}

int trans_data_server_main(int argc, char *argv[])
{
	struct trans_data_server_option option = { 0 };
	struct client_acceptor *client_acceptor = NULL;

	if (parse_option(argc, argv, &option) || validate_option(&option)) {
		print_usage();
		goto end;
	}

	if (initialize_trans_data_server(&option)) {
		goto end;
	}

	if (create_client_acceptor(&client_acceptor)) {
		goto end;
	}
	if (client_acceptor->open_socket(client_acceptor, option.port)) {
		goto end;
	}

	// for
	// connected_socket = client_acceptor->accept()

	// clients_manager->create_client_communicator(client_socket)

	// ]

end:
	return 0;
}


