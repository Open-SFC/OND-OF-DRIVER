include ../../../rules.mk



all: build

build: makebase makemmap makemheap

makebase:
	make -w -C $(utils_dir)/memory/base

makemmap:
#	make -w -C $(utils_dir)/memory/mmap

makemheap:
	make -w -C $(utils_dir)/memory/mheap



clean: cleanbase cleanmmap cleanmheap 

cleanbase:
	make -w -C $(utils_dir)/memory/base clean

cleanmmap:
#	make -w -C $(utils_dir)/memory/mmap clean

cleanmheap:
	make -w -C $(utils_dir)/memory/mheap clean
       


