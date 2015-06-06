include ../../../../rules.mk

$(BIN_DIR)/%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

TARGET = $(BIN_DIR)/traffic_steering-app.o

all: $(TARGET)

OBJS =  $(BIN_DIR)/tsc_init.o\
        $(BIN_DIR)/tsc_controller.o\
        $(BIN_DIR)/tsc_nvm_modules.o\
        $(BIN_DIR)/tsc_nsc_iface.o\
        $(BIN_DIR)/tsc_nsc_core.o\
        $(BIN_DIR)/tsc_nsc_cnbind.o\
        $(BIN_DIR)/tsc_nsc_notify.o\
        $(BIN_DIR)/tsc_nvm_vxlan.o\
        $(BIN_DIR)/tsc_vxn_views_init.o\
		$(BIN_DIR)/tsc_vxn_views_api.o\
        $(BIN_DIR)/nicira_tun_ext.o\
        $(BIN_DIR)/tsc_ofv1_3_plugin.o\
        $(BIN_DIR)/list_selectors.o\
        $(BIN_DIR)/list_repository.o\

$(TARGET) : $(OBJS) 
	$(LD) -r $(OBJS) -o $(TARGET) 

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJS)       
  
