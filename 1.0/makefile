include rules.mk
include $(cminfra_dir)/defs.system

all: buildall

OND_OF_DRVR_INST_DIR = /usr/local/ond_of_driver/1.0

#buildall: builducmcntrl buildcntrl builducmswitch buildswitch

switch: builducmswitch buildofswitch

controller: builducmcntrl buildcntrl buildpythonbindings

buildcntrl: 
	make -w -C $(cntrl_dir)

builducmcntrl:
	make -w -C $(cminfra_dir) -f makefile.cntrl

buildpythonbindings:
	make -w -C $(python_bindings_dir)

builducmswitch:
	make -w -C $(cminfra_dir) -f makefile.switch

buildofswitch:
	make -w -C $(switch_dir)

install:
	rm -rf /ondirector
	mkdir /ondirector
	mkdir /ondirector/bin
	mkdir /ondirector/debug
	mkdir /ondirector/conf
	mkdir /ondirector/app-conf
	mkdir /ondirector/certs
	mkdir /ondirector/html
	cp ondirector-start.sh /ondirector/
	cp ondirector-stop.sh /ondirector/
	cp -rf bin/lib  /ondirector/
	cp -f $(cminfra_dir)/obj/$(LIB_DIR)/*.so  /ondirector/lib
	cp -rf conf/on_director  /ondirector/conf
	cp -rf conf/cntrl-rconfig.xml  /ondirector/conf
	cp -rf controller/ofcli/system/html/* /ondirector/html/
	cp -f bin/controller  /ondirector/bin/
	cp -f bin/ofcli  /ondirector/bin/
	cp -f bin/ofucmd  /ondirector/bin/
	cp -f bin/ofldsvd  /ondirector/bin/
	cp -f bin/httpd  /ondirector/bin/
	cp -f certs/sc-cacert.pem  /ondirector/certs/
	cp -f certs/sc2-cacert.pem  /ondirector/certs/
	cp -f certs/ctl-privkey.pem  /ondirector/certs/
	cp -f certs/ctl-cert.pem  /ondirector/certs/

image: controllerimage switchimage

controllerimage:
	mkdir ondirector-install
	mkdir ondirector-install/bin
	mkdir ondirector-install/conf
	mkdir ondirector-install/app-conf
	mkdir ondirector-install/certs
	mkdir ondirector-install/html
	cp ondirector-start.sh ondirector-install/
	cp ondirector-install.sh ondirector-install/
	cp ondirector-stop.sh ondirector-install/
	cp -rf bin  ondirector-install/
	cp -rf $(cminfra_dir)/obj/$(LIB_DIR)/*  ondirector-install/bin/lib
	cp -rf certs  ondirector-install/
	cp -rf conf ondirector-install/
	cp -rf controller/ofcli/system/html/* ondirector-install/html/
	tar czf ondirector-image.tar.gz ondirector-install
	rm -rf ondirector-install

ctrlimageinstall:
	mkdir ondirector-install
	mkdir ondirector-install/bin
	mkdir ondirector-install/conf
	mkdir ondirector-install/app-conf
	mkdir ondirector-install/certs
	mkdir ondirector-install/html
	cp ond-of-start.sh ondirector-install/
	cp ond-of-stop.sh ondirector-install/
	cp -rf bin  ondirector-install/
	cp -rf $(cminfra_dir)/obj/$(LIB_DIR)/*  ondirector-install/bin/lib
	cp -rf certs  ondirector-install/
	cp -rf conf ondirector-install/
	cp -rf controller/ofcli/system/html/* ondirector-install/html/
	sudo rm -rf $(OND_OF_DRVR_INST_DIR)
	sudo mkdir -p $(OND_OF_DRVR_INST_DIR)
	sudo cp -rf ondirector-install/*  $(OND_OF_DRVR_INST_DIR)
	sudo cp -rf ondirector-install/bin/lib  $(OND_OF_DRVR_INST_DIR)
	sudo rm -rf ondirector-install/bin/lib
	rm -rf ondirector-install

ctrlhdrinstall:
	sudo rm -rf $(OND_OF_DRVR_INST_DIR)/include
	sudo mkdir  $(OND_OF_DRVR_INST_DIR)/include
	sudo cp -f controller/include/cntlr_error.h  $(OND_OF_DRVR_INST_DIR)/include
	sudo cp -f controller/include/cntlr_event.h  $(OND_OF_DRVR_INST_DIR)/include
	sudo cp -f controller/include/cntlr_system.h  $(OND_OF_DRVR_INST_DIR)/include
	sudo cp -f controller/include/cntrlappcmn.h  $(OND_OF_DRVR_INST_DIR)/include
	sudo cp -f controller/include/crmapi.h  $(OND_OF_DRVR_INST_DIR)/include
	sudo cp -f controller/include/dprm.h  $(OND_OF_DRVR_INST_DIR)/include
	sudo cp -f controller/include/mchash.h  $(OND_OF_DRVR_INST_DIR)/include
	sudo cp -f controller/include/memapi.h  $(OND_OF_DRVR_INST_DIR)/include
	sudo cp -f controller/include/nsrm.h  $(OND_OF_DRVR_INST_DIR)/include
	sudo cp -f controller/include/of_msgapi.h  $(OND_OF_DRVR_INST_DIR)/include
	sudo cp -f controller/include/of_multipart.h  $(OND_OF_DRVR_INST_DIR)/include
	sudo cp -f controller/include/oftype.h  $(OND_OF_DRVR_INST_DIR)/include
	sudo cp -f controller/include/of_utilities.h  $(OND_OF_DRVR_INST_DIR)/include
	sudo cp -f controller/include/timerapi.h  $(OND_OF_DRVR_INST_DIR)/include
	sudo cp -f controller/include/tsc_cli.h  $(OND_OF_DRVR_INST_DIR)/include
	sudo mkdir -p $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/common
	sudo cp -f cminfra/include/common/cmdefs.h  $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/common
	sudo cp -f cminfra/include/common/cmerror.h  $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/common
	sudo cp -f cminfra/include/common/cmincld.h  $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/common
	sudo cp -f cminfra/include/common/cmmath.h  $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/common
	sudo cp -f cminfra/include/common/cmtype.h  $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/common
	sudo mkdir -p $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/lxos
	sudo cp -f cminfra/include/lxos/cmatmic.h  $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/lxos
	sudo cp -f cminfra/include/lxos/cmoslck.h  $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/lxos
	sudo cp -f cminfra/include/lxos/cmsock.h  $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/lxos
	sudo cp -f cminfra/include/lxos/lxoswrp.h  $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/lxos
	sudo mkdir -p $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/infra/dm
	sudo cp -f cminfra/include/infra/dm/dmgdef.h  $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/infra/dm
	sudo cp -f cminfra/include/infra/dm/dmgif.h  $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/infra/dm
	sudo mkdir -p $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/infra/je
	sudo cp -f cminfra/include/infra/je/jegdef.h  $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/infra/je
	sudo mkdir -p $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/infra/transport
	sudo cp -f cminfra/include/infra/transport/cmtransgdf.h  $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/infra/transport
	sudo mkdir -p $(OND_OF_DRVR_INST_DIR)/include/ofproto/src/include
	sudo cp -f controller/ofproto/src/include/cntlr_epoll.h  $(OND_OF_DRVR_INST_DIR)/include/ofproto/src/include
	sudo cp -f controller/ofproto/src/include/cntlr_list.h  $(OND_OF_DRVR_INST_DIR)/include/ofproto/src/include
	sudo cp -f controller/ofproto/src/include/cntlr_lock.h  $(OND_OF_DRVR_INST_DIR)/include/ofproto/src/include
	sudo cp -f controller/ofproto/src/include/cntlr_tls.h  $(OND_OF_DRVR_INST_DIR)/include/ofproto/src/include
	sudo cp -f controller/ofproto/src/include/cntlr_transprt.h  $(OND_OF_DRVR_INST_DIR)/include/ofproto/src/include
	sudo cp -f controller/ofproto/src/include/cntlr_types.h  $(OND_OF_DRVR_INST_DIR)/include/ofproto/src/include
	sudo cp -f controller/ofproto/src/include/cntrl_queue.h  $(OND_OF_DRVR_INST_DIR)/include/ofproto/src/include
	sudo cp -f controller/ofproto/src/include/controller.h  $(OND_OF_DRVR_INST_DIR)/include/ofproto/src/include
	sudo cp -f controller/ofproto/src/include/crmldef.h  $(OND_OF_DRVR_INST_DIR)/include/ofproto/src/include
	sudo cp -f controller/ofproto/src/include/crm_port_api.h  $(OND_OF_DRVR_INST_DIR)/include/ofproto/src/include
	sudo cp -f controller/ofproto/src/include/crm_vm_api.h  $(OND_OF_DRVR_INST_DIR)/include/ofproto/src/include
	sudo cp -f controller/ofproto/src/include/crm_vn_api.h  $(OND_OF_DRVR_INST_DIR)/include/ofproto/src/include
	sudo cp -f controller/ofproto/src/include/dc_vs_ttp.h  $(OND_OF_DRVR_INST_DIR)/include/ofproto/src/include
	sudo cp -f controller/ofproto/src/include/dprmldef.h  $(OND_OF_DRVR_INST_DIR)/include/ofproto/src/include
	sudo cp -f controller/ofproto/src/include/nsrmldef.h  $(OND_OF_DRVR_INST_DIR)/include/ofproto/src/include
	sudo cp -f controller/ofproto/src/include/oflog.h  $(OND_OF_DRVR_INST_DIR)/include/ofproto/src/include
	sudo cp -f controller/ofproto/src/include/of_nem.h  $(OND_OF_DRVR_INST_DIR)/include/ofproto/src/include
	sudo cp -f controller/ofproto/src/include/of_view.h  $(OND_OF_DRVR_INST_DIR)/include/ofproto/src/include
	sudo cp -f controller/ofproto/src/include/openflow.h  $(OND_OF_DRVR_INST_DIR)/include/ofproto/src/include
	sudo cp -f controller/ofproto/src/include/port_status_mgr.h  $(OND_OF_DRVR_INST_DIR)/include/ofproto/src/include
	sudo mkdir -p $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/utils/dslib
	sudo cp -f cminfra/include/utils/dslib/cmdll.h  $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/utils/dslib
	sudo cp -f cminfra/include/utils/dslib/cmsll.h  $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/utils/dslib
	sudo cp -f cminfra/include/utils/dslib/cmmempol.h  $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/utils/dslib
	sudo mkdir -p $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/utils/netutil
	sudo cp -f cminfra/include/utils/netutil/cmnet.h  $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/utils/netutil
	sudo cp -f cminfra/include/utils/netutil/mbufutil.h  $(OND_OF_DRVR_INST_DIR)/include/cminfra/include/utils/netutil
	sudo mkdir -p $(OND_OF_DRVR_INST_DIR)/include/ucmcbk/cntlrucm/include
	sudo cp -f controller/ofcli/ucmcbk/cntlrucm/include/cntrucminc.h  $(OND_OF_DRVR_INST_DIR)/include/ucmcbk/cntlrucm/include/
	sudo cp -f controller/ofcli/ucmcbk/cntlrucm/include/cntrucmssl.h  $(OND_OF_DRVR_INST_DIR)/include/ucmcbk/cntlrucm/include/
	sudo mkdir -p $(OND_OF_DRVR_INST_DIR)/include/ucmcbk/comm/include
	sudo cp -f controller/ofcli/ucmcbk/comm/include/cmcntrlr.h  $(OND_OF_DRVR_INST_DIR)/include/ucmcbk/comm/include/
	sudo mkdir -p $(OND_OF_DRVR_INST_DIR)/include/ucmcbk/crm/include
	sudo cp -f controller/ofcli/ucmcbk/crm/include/crmcntrucmcbk.h  $(OND_OF_DRVR_INST_DIR)/include/ucmcbk/crm/include/
	sudo mkdir -p $(OND_OF_DRVR_INST_DIR)/include/ucmcbk/dprm/include
	sudo cp -f controller/ofcli/ucmcbk/dprm/include/cntrucmcbk.h  $(OND_OF_DRVR_INST_DIR)/include/ucmcbk/dprm/include/
	sudo mkdir -p $(OND_OF_DRVR_INST_DIR)/include/ucmcbk/includes/generated
	sudo cp -f controller/ofcli/ucmcbk/includes/generated/appids.h  $(OND_OF_DRVR_INST_DIR)/include/ucmcbk/includes/generated/
	sudo cp -f controller/ofcli/ucmcbk/includes/generated/secappl.h  $(OND_OF_DRVR_INST_DIR)/include/ucmcbk/includes/generated/
	sudo mkdir -p $(OND_OF_DRVR_INST_DIR)/include/ucmcbk/includes/global
	sudo cp -f controller/ofcli/ucmcbk/includes/global/cbcmninc.h  $(OND_OF_DRVR_INST_DIR)/include/ucmcbk/includes/global/
	sudo cp -f controller/ofcli/ucmcbk/includes/global/cbkerr.h  $(OND_OF_DRVR_INST_DIR)/include/ucmcbk/includes/global/
	sudo cp -f controller/ofcli/ucmcbk/includes/global/cbkldef.h  $(OND_OF_DRVR_INST_DIR)/include/ucmcbk/includes/global/
	sudo cp -f controller/ofcli/ucmcbk/includes/global/cmgutil.h  $(OND_OF_DRVR_INST_DIR)/include/ucmcbk/includes/global/
	sudo cp -f controller/ofcli/ucmcbk/includes/global/secappreg.h  $(OND_OF_DRVR_INST_DIR)/include/ucmcbk/includes/global/
	sudo cp -f controller/ofcli/ucmcbk/includes/global/cmreg.h  $(OND_OF_DRVR_INST_DIR)/include/ucmcbk/includes/global/
	sudo mkdir -p $(OND_OF_DRVR_INST_DIR)/include/third_party_opensrc/openssl-1.0.0/include/openssl
	sudo cp -f opensrc/openssl-1.0.0/include/openssl/ossl_typ.h  $(OND_OF_DRVR_INST_DIR)/include/third_party_opensrc/openssl-1.0.0/include/openssl
	sudo mkdir -p $(OND_OF_DRVR_INST_DIR)/include/third_party_opensrc/futex
	sudo cp -f opensrc/futex/usersem.h  $(OND_OF_DRVR_INST_DIR)/include/third_party_opensrc/futex

switchimage:
	mkdir onswitch
	mkdir onswitch/bin
	mkdir onswitch/conf
	mkdir onswitch/app-conf
	mkdir onswitch/certs
	mkdir onswitch/html
	cp onswitch-start.sh onswitch/
	cp switch-install.sh onswitch/
	cp onswitch-stop.sh onswitch/
	cp -rf bin  onswitch/
	cp -rf $(cminfra_dir)/obj/$(LIB_DIR)/*  onswitch/bin/lib
	cp -rf certs onswitch/
	cp -rf conf onswitch/
	cp -rf switch/mgmt/system/html/* onswitch/html/
	tar czf switch-image.tar.gz onswitch
	rm -rf onswitch
	make -w -C $(switch_dir) release
ifeq ($(TARGET_PLATFORM),p2041rdb)
	cp -rf $(switch_dir)/.p2041rdb-sdk1.3/_release/OFSwitchImage.sh .
endif
ifeq ($(TARGET_PLATFORM),p4080ds)
	cp -rf $(switch_dir)/.p4080ds-sdk1.3/_release/OFSwitchImage.sh .
endif
	tar czf onswitch-image.tar.gz OFSwitchImage.sh switch-image.tar.gz onswitch-install.sh
	rm -f OFSwitchImage.sh 

clean: cleanall

cleanall: cleanucmcntrl cleancntrl cleanucmswitch cleanswitchobj

cleancontroller: cleanucmcntrl cleancntrl

cleanswitch: cleanucmswitch cleanswitchobj

cleanucmcntrl:
	make -w -C $(cminfra_dir) -f makefile.cntrl clean
	rm -rf cntrl-image.tar.gz

cleanucmswitch:
	make -w -C $(cminfra_dir) -f makefile.switch clean
	rm -rf switch-image.tar.gz

cleancntrl:
	make -w -C $(cntrl_dir) clean

cleanswitchobj:
	make -w -C $(switch_dir) clean

