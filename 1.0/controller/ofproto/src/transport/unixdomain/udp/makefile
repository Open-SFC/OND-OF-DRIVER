include ../../../../../../rules.mk


$(BIN_DIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

SHARED_LIB = $(OF_LIB_DIR)/proto

TARGET_LIB = $(SHARED_LIB)/s06-unix-domain-udp-transport.so

all: $(TARGET_LIB)

OBJS =  $(BIN_DIR)/unix_dm_udp_init.o\
        $(BIN_DIR)/unix_dm_udp_api.o

$(TARGET_LIB) : $(OBJS)
	$(CC) -shared -o $@ $(OBJS) -lm

clean:
	$(RM) $(TARGET_LIB)
	$(RM) $(OBJS)    
