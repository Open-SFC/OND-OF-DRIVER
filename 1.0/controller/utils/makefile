include ../../rules.mk

all:build

$(BIN_DIR)/%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

SHARED_LIB = $(OF_LIB_DIR)

TARGET_LIB = $(SHARED_LIB)/libctlrutils.so
TARGET = $(BIN_DIR)/ctlrutils.o

build: makememory makepktmbuf $(TARGET) $(TARGET_LIB)

OBJS = 	$(BIN_DIR)/mchash.o \
	$(BIN_DIR)/epoll.o \
	$(BIN_DIR)/queue.o \
	$(BIN_DIR)/ofmem.o \
	$(BIN_DIR)/msgpool.o \
	$(BIN_DIR)/sll.o \
	$(BIN_DIR)/oflog.o \
	$(BIN_DIR)/trace.o \
	$(BIN_DIR)/timer.o \
	$(BIN_DIR)/dll.o

$(TARGET) : $(OBJS) 
	$(LD) -r $(OBJS) -o $(TARGET) 

$(TARGET_LIB) : $(OBJS)
	$(CC) -shared -o $@ $(OBJS) -lm -ldl

makememory:
	make -w -C $(utils_dir)/memory
makepktmbuf:
#	make -w -C $(utils_dir)/pktmbuf



clean:
	make -w -C $(utils_dir)/memory clean
#	make -w -C $(utils_dir)/pktmbuf clean
	$(RM) $(TARGET_LIB)
	$(RM) $(TARGET)
	$(RM) $(OBJS)
       


