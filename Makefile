.PHONY: build compile ff

compile:
	make -C ./build

build:
	mkdir -p build;
	cd build && cmake ../

ff:
	cd lib/fastflow-master/ff && ./mapping_string.sh

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

testall:
	./test.sh SD 0;
	./test.sh SD 1;
	./test.sh SD 2;
	./test.sh HD 0;
	./test.sh HD 1;
	./test.sh HD 2;
	./test.sh FHD 0;
	./test.sh FHD 1;
	./test.sh FHD 2;