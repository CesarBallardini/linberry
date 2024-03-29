#!/bin/sh

if [ -z "$1" -o -z "$2" -o -z "$3" ] ; then
	echo
	echo "Usage: ./test.sh MAJOR MINOR commit"
	echo
	echo "Creates the release tarball from git sources, tests the compile"
	echo "on local machine, fedora 5, 6, and 7, and opensuse 10.2."
	echo
	exit 1
fi

set -e

# Create the tarball
./git-release-tar.sh $1 $2 $3

# Make sure it compiles cleanly on all handy systems
# Local first...
./test-build-local.sh build/linberry-$1.$2.tar.bz2

# Then as root, for the chroot systems...
su - -c "cd $(pwd) && ./test-root.sh $1 $2"

