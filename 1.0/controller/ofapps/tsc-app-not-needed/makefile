include ../../../rules.mk

SHARED_LIB = $(OF_LIB_DIR)/apps

TARGET_LIB = $(SHARED_LIB)/s14-traffic-steering-app.so

all: makesrc maketest maketarget

makesrc: 
	make -w -C $(ofapps_dir)/tsc-app/src

maketest: 
	make -w -C $(ofapps_dir)/tsc-app/test

maketarget:$(TARGET_LIB)

OBJS =  $(BIN_DIR)/traffic_steering-app.o\
        $(BIN_DIR)/traffic_steering-test.o\

$(TARGET_LIB) : $(OBJS)
	$(CC) -shared -o $@ $(OBJS) -lm

clean:
	make -C $(ofapps_dir)/tsc-app/src clean
	make -C $(ofapps_dir)/tsc-app/test clean
	$(RM) $(TARGET_LIB)
