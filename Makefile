.PHONY: build compile

compile:
	make -C ./build

build:
	mkdir -p build;
	cd build && cmake ../

testSDseq:
	./test.sh SD 0;

testSDfarm:
	./test.sh SD 1;

testSDpipe:
	./test.sh SD 2;

testHDseq:
	./test.sh HD 0;

testHDfarm:
	./test.sh HD 1;

testHDpipe:
	./test.sh HD 2;

testFHDseq:
	./test.sh FHD 0;

testFHDfarm:
	./test.sh FHD 1;

testFHDpipe:
	./test.sh FHD 2;