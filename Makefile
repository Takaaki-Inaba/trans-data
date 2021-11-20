CC	= gcc
CFLAGS	= -Wall -Wconversion -g
LIBS = -lpthread

CM_DIR = common
SV_DIR = server
CL_DIR = client
BIN_DIR = bin

SV_PROGRAM = trans-data-server
CL_PROGRAM = trans-data-client

CM_OBJS = $(CM_DIR)/debug.o $(CM_DIR)/message_parser.o $(CM_DIR)/utility.o
SV_OBJS = $(SV_DIR)/main.o $(SV_DIR)/server_main.o $(SV_DIR)/client_acceptor.o $(SV_DIR)/communicate_client.o 

$(SV_PROGRAM): $(CM_OBJS) $(SV_OBJS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $^ $(LIBS)

all: $(SV_PROGRAM)

$(SV_PROGRAM): | $(BIN_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

.PHONY: clean
clean:
	rm -r $(BIN_DIR)
	rm $(CM_OBJS)
	rm $(SV_OBJS)

