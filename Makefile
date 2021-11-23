CC	= gcc
CFLAGS	= -Wall -Wconversion -g 
LIBS = -lpthread

CM_DIR = common
SV_DIR = server
CL_DIR = client
BIN_DIR = bin

SV_PROGRAM = trans-data-server
CL_PROGRAM = trans-data-client

CM_OBJS = $(CM_DIR)/debug.o $(CM_DIR)/message_lib.o $(CM_DIR)/utility.o
SV_OBJS = $(SV_DIR)/main.o $(SV_DIR)/server_main.o $(SV_DIR)/server_session.o $(SV_DIR)/server_socket.o $(SV_DIR)/communicate_client.o 
CL_OBJS = $(CL_DIR)/main.o $(CL_DIR)/client_main.o $(CL_DIR)/client_session.o $(CL_DIR)/client_socket.o $(CL_DIR)/communicate_server.o

all: $(SV_PROGRAM) $(CL_PROGRAM)

$(SV_PROGRAM): $(CM_OBJS) $(SV_OBJS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $^ $(LIBS)

$(CL_PROGRAM): $(CM_OBJS) $(CL_OBJS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $^ $(LIBS)

$(SV_PROGRAM): | $(BIN_DIR)
$(CL_PROGRAM): | $(BIN_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

.PHONY: clean
clean:
	rm -rf $(BIN_DIR)
	rm -f $(CM_OBJS)
	rm -f $(SV_OBJS)
	rm -f $(CL_OBJS)

