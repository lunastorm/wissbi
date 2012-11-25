.PHONY:	test build

default: build

tmp/udt4:
	mkdir -p tmp/
	cp 3rd_party/udt.sdk.4.10.tar.gz tmp
	cd tmp ; tar -zxvf udt.sdk.4.10.tar.gz
	cd tmp/udt4 ; make
	rm -f tmp/udt.sdk.4.10.tar.gz

tmp/gtest-1.6.0:
	mkdir -p tmp/
	cp 3rd_party/gtest-1.6.0.zip tmp
	cd tmp ; unzip gtest-1.6.0.zip
	cd tmp/gtest-1.6.0 ; cmake . && make
	rm -f tmp/gtest-1.6.0.zip

build: tmp/udt4
	mkdir -p tmp/build
	cd tmp/build && cmake ../../src && make

test: tmp/gtest-1.6.0
	mkdir -p tmp/test
	cd tmp/test && cmake ../../test && make && ./runUnitTests

clean:
	rm -rf tmp
