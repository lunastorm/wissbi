INSTALL_PREFIX	?=
BUILD_NUM		?=	0
DEB_ARCH		:=  amd64
VERSION			:=  0.9
DEB_VERSION		:=	$(VERSION).$(BUILD_NUM)+$(shell git log --pretty=format:'%h' -n 1)
DEBBUILD_DIR	:=  tmp/wissbi-$(DEB_VERSION)_$(DEB_ARCH)

.PHONY:	test build

default: build

3rd_party/libcxx:
	svn co http://llvm.org/svn/llvm-project/libcxx/tags/RELEASE_351/final/ $@

3rd_party/libcxxabi:
	svn co http://llvm.org/svn/llvm-project/libcxxabi/tags/RELEASE_351/final/ $@

3rd_party/gmock-1.6.0:
	cd 3rd_party ; unzip gmock-1.6.0.zip

tmp/test/3rd_party/shunit2:
	mkdir -p tmp/test/3rd_party
	cp -r 3rd_party/shunit2 tmp/test/3rd_party/

build: 3rd_party/gmock-1.6.0 3rd_party/libcxx 3rd_party/libcxxabi
	mkdir -p tmp/build
	cd tmp/build && cmake -Wno-dev -DCMAKE_BUILD_TYPE=Release -DLIBCXX_ENABLE_SHARED=off ../../ && make -j`nproc` wissbi-pub wissbi-sub wissbi-count wissbi-record

test: 3rd_party/gmock-1.6.0 tmp/test/3rd_party/shunit2
	mkdir -p tmp/test
	mkdir -p output/test
	if ! grep WISSBI_LINK_LIBS 3rd_party/gmock-1.6.0/CMakeLists.txt ;\
		then sed -i -e 's/^\(target_link_libraries(.*\))/\1 $${WISSBI_LINK_LIBS})/' 3rd_party/gmock-1.6.0/CMakeLists.txt ; fi
	cd tmp/test && cmake -DCMAKE_BUILD_TYPE=Debug ../../ && make -j`nproc` && make test ; mv test/test_*.xml ../../output/test

clean:
	rm -rf tmp
	rm -rf output

clean-all:
	rm -rf tmp
	rm -rf output
	rm -rf 3rd_party/libcxx
	rm -rf 3rd_party/libcxxabi

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
	cp -f 3rd_party/ganglia-python/gmetric.py $(INSTALL_PREFIX)/usr/bin
	cp -f contrib/wissbi_flow_graph.py $(INSTALL_PREFIX)/usr/bin
	cp -f contrib/wissbi_flow_dump.py $(INSTALL_PREFIX)/usr/bin
	cp -rf doc/* $(INSTALL_PREFIX)/usr/share/doc/wissbi

.PHONY:	deb
deb:
	rm -rf $(DEBBUILD_DIR)
	INSTALL_PREFIX=$(DEBBUILD_DIR) make install
	mkdir -p $(DEBBUILD_DIR)/DEBIAN
	cd $(DEBBUILD_DIR) ; find . -type f | sed -e '/DEBIAN\/md5sums/d' | xargs md5sum | sed -e 's/\.\///g' > DEBIAN/md5sums
	cp -f pkg/deb/* $(DEBBUILD_DIR)/DEBIAN/
	sed -i -e 's/{arch}/$(DEB_ARCH)/g; s/{version}/$(DEB_VERSION)/g' $(DEBBUILD_DIR)/DEBIAN/control
	dpkg-deb --build $(DEBBUILD_DIR)
	mkdir -p output/artifacts
	mv tmp/wissbi-*.deb output/artifacts/

