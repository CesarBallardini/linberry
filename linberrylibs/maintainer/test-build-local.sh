#!/bin/sh

CHROOT="/var/chroot"
BUILDUSER="cdfrey"

if [ -z "$1" ] ; then
	echo
	echo "Usage: ./test-build-local.sh tarball"
	echo
	echo "Does a full build of given tarball."
	echo
	exit 1
fi

TARPATH="$1"
TARNAME=`basename "$TARPATH"`
TARFULLPATH="$(pwd)/$TARPATH"
TARGET="$2"

set -e

#
# perform the build test in 3 stages, so that we test the
# headers are valid for non-linberry tree builds (i.e. can we build
# properly with headers installed in /usr/include/linberry ?)
#
cd
rm -rf testbuild
mkdir testbuild

cd testbuild
tar xjvf $TARFULLPATH

cd *
./configure --prefix=/home/$BUILDUSER/testbuild/rootdir
make install

cd gui
export PKG_CONFIG_PATH=/home/$BUILDUSER/testbuild/rootdir/lib/pkgconfig
./configure --prefix=/home/$BUILDUSER/testbuild/rootdir
make install

cd ../opensync-plugin
./configure --prefix=/home/$BUILDUSER/testbuild/rootdir
make
#make install	# not included, since opensync's install dest depends on target

cd ../../rootdir
find
echo "Press enter to continue..."
read

cd ../..
rm -rf testbuild

