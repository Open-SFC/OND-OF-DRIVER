include ../../../rules.mk


$(BIN_DIR)/%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)


TARGET = $(BIN_DIR)/ctlrtest.o

all: $(TARGET) 

OBJS = 	$(BIN_DIR)/cbench_app.o\
	$(BIN_DIR)/dprmtest.o
#	$(BIN_DIR)/cntlr_test.o\
#	$(BIN_DIR)/test_configapp.o

$(TARGET) : $(OBJS) 
	$(LD) -r $(OBJS) -o $(TARGET) 

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJS)
       


