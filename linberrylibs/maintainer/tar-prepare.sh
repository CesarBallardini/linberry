#!/bin/sh

if [ -n "$1" ] ; then
	echo
	echo "Usage: ./tar-prepare.sh"
	echo
	echo "This script prepares a Linberry directory for release, assuming"
	echo "that it is run from inside the root of a freshly extracted"
	echo "Linberry source tree."
	echo
	exit 1
fi

set -e

# Are we in a Linberry root tree?
if ! [ -f AUTHORS -a -f ChangeLog -a -f COPYING -a -f buildgen.sh ] ; then
	echo "Can't find Linberry files. Please run from root of Linberry tree."
	exit 1
fi

# Generate web docs
(cd doc/www && ./static.sh)

# Generate doxygen docs
(doxygen && rm -f doxygen.log)

# Generate configure scripts
./buildgen.sh

# Clean up the extraneous autoconf clutter
rm -rf \
	autom4te.cache \
	gui/autom4te.cache \
	opensync-plugin/autom4te.cache \
	opensync-plugin-0.4x/autom4te.cache

