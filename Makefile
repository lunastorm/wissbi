.PHONY:	test build

default: build

tmp/gmock-1.6.0:
	mkdir -p tmp/
	cp 3rd_party/gmock-1.6.0.zip tmp
	cd tmp ; unzip gmock-1.6.0.zip
	cd tmp/gmock-1.6.0 ; cmake . && make
	rm -f tmp/gmock-1.6.0.zip

tmp/3rd_party/shunit2:
	mkdir -p tmp/3rd_party
	cp -r 3rd_party/shunit2 tmp/3rd_party/

build:
	mkdir -p tmp/build
	cd tmp/build && cmake ../../src && make

test: tmp/gmock-1.6.0 build tmp/3rd_party/shunit2
	mkdir -p tmp/test
	mkdir -p output/test
	cd tmp/test && cmake ../../test && make && make test ; mv test_*.xml ../../output/test

clean:
	rm -rf tmp
	rm -rf output
