include ../../../../rules.mk



all: build

build: makebase maketcp makeudp makevirtio maketls makeunixdm
#build: makebase makeudp

makebase:
	make -w -C $(ofproto_dir)/src/transport/base

maketcp:
	make -w -C $(ofproto_dir)/src/transport/tcp

makeudp:
	make -w -C $(ofproto_dir)/src/transport/udp

maketls:
	make -w -C $(ofproto_dir)/src/transport/tls

makevirtio:
	make -w -C $(ofproto_dir)/src/transport/virtio

makeunixdm:
	make -w -C $(ofproto_dir)/src/transport/unixdomain


clean: cleanbase cleantcp cleanudp cleanvirtio cleantls cleanunixdm

cleanbase:
	make -w -C $(ofproto_dir)/src/transport/base clean
cleantcp:
	make -w -C $(ofproto_dir)/src/transport/tcp clean
cleanudp:
	make -w -C $(ofproto_dir)/src/transport/udp clean
cleantls:
	make -w -C $(ofproto_dir)/src/transport/tls clean
cleanvirtio:
	make -w -C $(ofproto_dir)/src/transport/virtio clean
cleanunixdm:
	make -w -C $(ofproto_dir)/src/transport/unixdomain clean
       


