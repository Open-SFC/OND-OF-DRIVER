include ../../../../../rules.mk


all: build

build: makeudmtcp makeudmudp 


makeudmtcp:
	make -w -C $(ofproto_dir)/src/transport/unixdomain/tcp

makeudmudp:
	make -w -C $(ofproto_dir)/src/transport/unixdomain/udp

clean: cleanudmtcp cleanudmudp 

cleanudmtcp:
	make -w -C $(ofproto_dir)/src/transport/unixdomain/tcp clean
cleanudmudp:
	make -w -C $(ofproto_dir)/src/transport/unixdomain/udp clean

