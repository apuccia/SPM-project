.PHONY: build compile

compile:
	make -C ./build

build:
	mkdir -p build;
	cd build && cmake ../
