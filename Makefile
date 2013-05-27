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
	cd tmp/build && cmake -DCMAKE_BUILD_TYPE=Release ../../ && make -j`nproc` wissbi-pub wissbi-sub wissbi-count wissbi-record

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
	mkdir -p $(INSTALL_PREFIX)/etc/init.d
	mkdir -p $(INSTALL_PREFIX)/etc/bash_completion.d
	mkdir -p $(INSTALL_PREFIX)/usr/share/doc/wissbi
	cp -f tmp/build/src/wissbi-pub $(INSTALL_PREFIX)/usr/bin
	cp -f tmp/build/src/wissbi-sub $(INSTALL_PREFIX)/usr/bin
	cp -f tmp/build/src/wissbi-count $(INSTALL_PREFIX)/usr/bin
	cp -f tmp/build/src/wissbi-record $(INSTALL_PREFIX)/usr/bin
	cp -f scripts/wissbi-bash_completion.sh $(INSTALL_PREFIX)/etc/bash_completion.d
	cp -f scripts/wsbmetad.sh $(INSTALL_PREFIX)/usr/bin
	cp -f scripts/wsbmetad $(INSTALL_PREFIX)/etc/init.d
	cp -f scripts/wissbi_filter_template.sh $(INSTALL_PREFIX)/usr/bin
	cp -f scripts/wdb $(INSTALL_PREFIX)/usr/bin
	cp -f scripts/wsbmetric.py $(INSTALL_PREFIX)/usr/bin
	cp -f scripts/wsbmetricd $(INSTALL_PREFIX)/etc/init.d
	cp -f scripts/wsblogcollectord $(INSTALL_PREFIX)/etc/init.d
	cp -f scripts/wissbi_log_collector.sh $(INSTALL_PREFIX)/usr/bin
	cp -f scripts/wissbi_log_transformer.py $(INSTALL_PREFIX)/usr/bin
	cp -f scripts/wsbforwarderd $(INSTALL_PREFIX)/etc/init.d
	cp -rf doc/* $(INSTALL_PREFIX)/usr/share/doc/wissbi

.PHONY:	deb
deb:
	rm -rf $(DEBBUILD_DIR)
	INSTALL_PREFIX=$(DEBBUILD_DIR) make install
	mkdir -p $(DEBBUILD_DIR)/DEBIAN
	cd $(DEBBUILD_DIR) ; find . -type f | sed -e '/DEBIAN\/md5sums/d' | xargs md5sum | sed -e 's/\.\///g' > DEBIAN/md5sums
	cp -f pkg/deb/* $(DEBBUILD_DIR)/DEBIAN/
	sed -i -e 's/^Architecture:.*/Architecture: amd64/' $(DEBBUILD_DIR)/DEBIAN/control
	dpkg-deb --build $(DEBBUILD_DIR)
	mkdir -p output/artifacts
	mv tmp/wissbi-*.deb output/artifacts/

