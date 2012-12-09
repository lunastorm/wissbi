.PHONY:	test build

default: build

tmp/gmock-1.6.0:
	mkdir -p tmp/
	cp 3rd_party/gmock-1.6.0.zip tmp
	cd tmp ; unzip gmock-1.6.0.zip
	cd tmp/gmock-1.6.0 ; cmake . && make
	rm -f tmp/gmock-1.6.0.zip

build:
	mkdir -p tmp/build
	cd tmp/build && cmake ../../src && make

test: tmp/gmock-1.6.0
	mkdir -p tmp/test
	mkdir -p output/test
	cd tmp/test && cmake ../../test && make && make test ; mv test_detail.xml ../../output/test

clean:
	rm -rf tmp
	rm -rf output
