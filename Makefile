.PHONY: build compile

compile:
	make -C ./build

build:
	mkdir -p build;
	cd build && cmake ../

testSD:
	./test.sh SD 0;
	./test.sh SD 1;
	./test.sh SD 2;

testHD:
	./test.sh HD 0;
	./test.sh HD 1;
	./test.sh HD 2;

testFHD:
	./test.sh FHD 0;
	./test.sh FHD 1;
	./test.sh FHD 2;