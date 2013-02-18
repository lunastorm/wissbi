INSTALL_PREFIX	?=
DEBBUILD_DIR	:=	tmp/wissbi-0.1-$(shell date -u "+%Y%m%d")_amd64

.PHONY:	test build

default: build

tmp/gmock-1.6.0:
	mkdir -p tmp/
	cp 3rd_party/gmock-1.6.0.zip tmp
	cd tmp ; unzip gmock-1.6.0.zip
	cd tmp/gmock-1.6.0 ; env CXX="/usr/bin/clang++" CXXFLAGS="-std=c++11 -stdlib=libc++ -DGTEST_USE_OWN_TR1_TUPLE" cmake . && make
	rm -f tmp/gmock-1.6.0.zip

tmp/3rd_party/shunit2:
	mkdir -p tmp/3rd_party
	cp -r 3rd_party/shunit2 tmp/3rd_party/

build:
	mkdir -p tmp/build
	cd tmp/build && env CXX="/usr/bin/clang++" CXXFLAGS="-std=c++11 -stdlib=libc++" cmake ../../src && make

test: tmp/gmock-1.6.0 build tmp/3rd_party/shunit2
	mkdir -p tmp/test
	mkdir -p output/test
	cd tmp/test && env CXX="/usr/bin/clang++" CXXFLAGS="-std=c++11 -stdlib=libc++ -DGTEST_USE_OWN_TR1_TUPLE" cmake ../../test && make && make test ; mv test_*.xml ../../output/test

clean:
	rm -rf tmp
	rm -rf output

.PHONY:	install
install: build
	mkdir -p $(INSTALL_PREFIX)/usr/bin
	mkdir -p $(INSTALL_PREFIX)/etc/bash_completion.d
	cp -f tmp/build/wissbi-pub $(INSTALL_PREFIX)/usr/bin
	cp -f tmp/build/wissbi-sub $(INSTALL_PREFIX)/usr/bin
	cp -f tmp/build/wissbi-count $(INSTALL_PREFIX)/usr/bin
	cp -f scripts/wissbi-bash_completion.sh $(INSTALL_PREFIX)/etc/bash_completion.d
	cp -f scripts/wsbmetad.sh $(INSTALL_PREFIX)/usr/bin

.PHONY:	deb
deb:
	rm -rf $(DEBBUILD_DIR)
	INSTALL_PREFIX=$(DEBBUILD_DIR) make install
	mkdir -p $(DEBBUILD_DIR)/DEBIAN
	cd $(DEBBUILD_DIR) ; find . -type f | sed -e '/DEBIAN\/md5sums/d' | xargs md5sum | sed -e 's/\.\///g' > DEBIAN/md5sums
	cp -f pkg/deb/* $(DEBBUILD_DIR)/DEBIAN/
	dpkg-deb --build $(DEBBUILD_DIR)
	mkdir -p output/artifacts
	mv tmp/wissbi-*.deb output/artifacts/

