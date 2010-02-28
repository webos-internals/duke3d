NAME = duke3d
VERSION = 1.0.0


include ../../support/download.mk

.PHONY: unpack
unpack: build/.unpacked

build/.unpacked:
	rm -rf build
	mkdir -p build
	touch $@

include ../../support/staging.mk

stage::

include ../../support/cross-compile.mk

build: build/armv7.built

build/%.built: build/.unpacked
	rm -f $@
	( cd src/jfbuild ; ${SB2} make)
	( cd src/jfduke3d ; ${SB2} make)
	cp src/jfduke3d/duke3d .
	mkdir src/binaries
	cp src/jfduke3d/duke3d src/binaries
	touch $@

clobber::
	rm -rf build
