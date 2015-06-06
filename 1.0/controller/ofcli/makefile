include ../../rules.mk
CM_DIR = $(cminfra_dir)
include $(CM_DIR)/rules.mod

all: build

makedmtool:
	ant -f $(CM_DM_TOOL_BIN_DIR)/build.xml -DPROD_DIR=$(CM_PROD_DIR) -DCM_PROJECT_PATH_FORANT=$(CM_PROJECT_PATH_FORANT) -DGLU_PROD_DIR=$(GLU_PROD_DIR)

makecbk: 
	make -C ucmcbk

build: makedmtool makecbk 

clean: cleanothers cleandmtool
cleanothers:
	make -C ucmcbk clean
	$(RM) $(MODSET_OBJ_DIR)/*.o
	$(RM) $(OF_LIB_DIR)/libcntrlucmcbk.so

cleandmtool:
	$(RM) $(ofcli_dir)/ucmcbk/includes/generated/*.h
       
