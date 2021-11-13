#include <limits.h>

#include "../lib/debug.h"
#include "trans_data_server_main.h"

struct option {
#define PORT_NUMBER_MAX_LEN 5
	char port[PORT_NUMBER_MAX_LEN + 1];
	char files_store_dir_path[PATH_MAX];
};

int trans_data_server_main(int argc, char *argv[])
{
	// parse_option

	// validate_option

	// trans_data_server_init block signal/daemon/debug

	// create_server

	// create_client_communication_manager
	// managerは別スレッドで起動する。client_socketを受け取って、workerを作成する

	// for
	// client_socket = sever->accept()

	// server->is_received_terminate(client_socket)

	// ccm->create_client_communicater(client_socket)

	// ]

	// ccm->terminate
}


