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
	( cd src/jfbuild ; ${SB2} make)
	( cd src/jfduke3d ; ${SB2} make)
	cp src/jfduke3d/duke3d .
	touch $@

clobber::
	rm -rf build
