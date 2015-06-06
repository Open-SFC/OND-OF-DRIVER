include ../../rules.mk


$(BIN_DIR)/%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

SHARED_LIB = $(OF_LIB_DIR)/apps

TARGET1 = $(SHARED_LIB)/s01-l3ext-utils.so

all: $(TARGET1)

OBJS =  $(BIN_DIR)/of_l3ext_utils.o \
        $(BIN_DIR)/nicira_ext.o

$(TARGET1) : $(OBJS)
	$(CC) -shared -o $@ $(OBJS) -lm

clean:
	$(RM) $(TARGET1)
	$(RM) $(OBJS)       


