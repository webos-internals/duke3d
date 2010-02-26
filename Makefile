NAME = Duke3D
VERSION = 0.00

include ../../support/download.mk

.PHONY: unpack
unpack: build

build/.unpacked:

include ../../support/staging.mk

stage::

build: build/armv7.built

include ../../support/staging.mk
include ../../support/cross-compile.mk


build/%.built: build/.unpacked
	rm -f $@
	( cd jfbuild ; ${SB2} make)
	( cd jfduke3d ; ${SB2} make)
	touch $@

clobber::
	rm -rf build
