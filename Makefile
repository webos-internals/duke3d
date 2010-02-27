NAME     = duke3d
APP_ID   = org.webosinternals.${NAME}
SIGNER   = org.webosinternals
VERSION  = 1.0.0
DEPENDS  = org.webosinternals.upstartmgr
HOMEPAGE = http://www.webos-internals.org/wiki/Application:Duke3D
MAINTAINER = Henk Jonas
TITLE   = Duke3D
TYPE	= Application
DESCRIPTION = Port of the popular Duke Nukem 3D shooter game written by Real Software.<br>It requires webOS 1.3.5 or later.
CHANGELOG = 1.0.0: Control overlay. Initial release.
CATEGORY = Games
ICON	= http://www.webos-internals.org/images/3/3a/Quake-icon3-resized.png
LICENSE  = GNU License
SCREENSHOTS = [\
\"http://www.webos-internals.org/images/2/26/Quake_Menu.png\",\
\"http://www.webos-internals.org/images/4/4b/Quake_Lava.png\",\
\"http://www.webos-internals.org/images/6/66/Quake_Ogre.png\"\
 ]

#SRC_GIT = git://git.webos-internals.org/games/duke3d.git

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

include ../../support/package.mk
package: ipkgs/${APP_ID}_${VERSION}_armv7.ipk

clobber::
	rm -rf build
