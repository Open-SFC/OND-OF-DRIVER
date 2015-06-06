include ../../../../../rules.mk


$(BIN_DIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

SHARED_LIB = $(OF_LIB_DIR)/proto

TARGET_LIB = $(SHARED_LIB)/s02-tls-transport.so

all: $(TARGET_LIB)

OBJS =  $(BIN_DIR)/tls_init.o\
        $(BIN_DIR)/tls_api.o\
        $(BIN_DIR)/tls_tcp_api.o

$(TARGET_LIB) : $(OBJS)
	$(CC) -shared -o $@ $(OBJS) -lm

clean:
	$(RM) $(TARGET_LIB)
	$(RM) $(OBJS)    
