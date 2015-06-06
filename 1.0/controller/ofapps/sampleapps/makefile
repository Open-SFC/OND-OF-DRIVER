include ../../../rules.mk


$(BIN_DIR)/%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

SHARED_LIB = $(OF_LIB_DIR)/apps

TARGET_LIB = $(SHARED_LIB)/s02-sample-app.so

all: $(TARGET_LIB)

OBJS =  $(BIN_DIR)/cntlr_sample_app.o\
        $(BIN_DIR)/cntlr_sm_arp_app.o\
        $(BIN_DIR)/sample_app_init.o\
	    $(BIN_DIR)/cntlr_sm_app_igmp.o\
        $(BIN_DIR)/sample_l2_init.o

$(TARGET_LIB) : $(OBJS)
	$(CC) -shared -o $@ $(OBJS) -lm

clean:
	$(RM) $(TARGET_LIB)
	$(RM) $(OBJS)       


