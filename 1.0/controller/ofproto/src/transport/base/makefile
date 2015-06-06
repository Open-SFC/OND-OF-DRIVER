include ../../../../../rules.mk
UCMCBK_DIR= $(ofcli_dir)/ucmcbk
include $(UCMCBK_DIR)/rules.mod

$(BIN_DIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)


TARGET = $(BIN_DIR)/transport.o

all: $(TARGET)

OBJS =  $(BIN_DIR)/trnsprt_init.o\
        $(BIN_DIR)/trnsprt_api.o\
        $(BIN_DIR)/trnsprt_fsm.o\
        $(BIN_DIR)/trnsprt_channel.o\
        $(BIN_DIR)/trnsprt_epoll.o\
        $(BIN_DIR)/trnsprt_thread.o\
        $(BIN_DIR)/trnsprt_msg.o\
        $(BIN_DIR)/channel_fsm.o\
        $(BIN_DIR)/tls_core.o\
        $(BIN_DIR)/trnsprt_core.o

$(TARGET) : $(OBJS)
	$(LD) -r $(OBJS) -o $(TARGET) 


clean:
	$(RM) $(TARGET)
	$(RM) $(OBJS)    
