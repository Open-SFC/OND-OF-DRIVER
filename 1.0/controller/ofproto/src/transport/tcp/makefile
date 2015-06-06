include ../../../../../rules.mk


$(BIN_DIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

SHARED_LIB = $(OF_LIB_DIR)/proto

TARGET_LIB = $(SHARED_LIB)/s01-tcp-transport.so

all: $(TARGET_LIB)

OBJS =  $(BIN_DIR)/tcp_init.o\
        $(BIN_DIR)/tcp_api.o

$(TARGET_LIB) : $(OBJS)
	$(CC) -shared -o $@ $(OBJS) -lm

clean:
	$(RM) $(TARGET_LIB)
	$(RM) $(OBJS)    
