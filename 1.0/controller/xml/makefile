include ../../rules.mk


$(BIN_DIR)/%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

SHARED_LIB = $(OF_LIB_DIR)

TARGET_LIB = $(SHARED_LIB)/libctlrxml.so
TARGET = $(BIN_DIR)/ctlrxml.o

all: $(TARGET) $(TARGET_LIB)

OBJS = 	$(BIN_DIR)/attr.o \
	$(BIN_DIR)/chvalid.o \
	$(BIN_DIR)/document.o \
	$(BIN_DIR)/element.o \
	$(BIN_DIR)/error.o \
	$(BIN_DIR)/globals.o \
	$(BIN_DIR)/hash.o \
	$(BIN_DIR)/ixml.o \
	$(BIN_DIR)/ixmlmembuf.o \
	$(BIN_DIR)/ixmlparser.o \
	$(BIN_DIR)/namednodemap.o \
	$(BIN_DIR)/node.o \
	$(BIN_DIR)/nodelist.o \
	$(BIN_DIR)/parserutil.o \
	$(BIN_DIR)/pspxml.o \
	$(BIN_DIR)/threads.o \
	$(BIN_DIR)/xmlfutil.o \
	$(BIN_DIR)/xmlhandler.o \
	$(BIN_DIR)/xmlregexp.o \
	$(BIN_DIR)/xmlrgstr.o \
	$(BIN_DIR)/xmlschemas.o \
	$(BIN_DIR)/xmlschemastypes.o \
	$(BIN_DIR)/xmlstrbuffutil.o \
	$(BIN_DIR)/xmlstring.o \
	$(BIN_DIR)/xmlunicode.o \
	$(BIN_DIR)/cntrlres.o
	#$(BIN_DIR)/testxml.o \
	#$(BIN_DIR)/testschemas.o \
	$(BIN_DIR)/dll.o

$(TARGET) : $(OBJS) 
	$(LD) -r $(OBJS) -o $(TARGET) 

$(TARGET_LIB) : $(OBJS)
	$(CC) -shared -o $@ $(OBJS) -lm -ldl
clean:
	$(RM) $(TARGET_LIB)
	$(RM) $(TARGET)
	$(RM) $(OBJS)

