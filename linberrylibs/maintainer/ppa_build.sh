#!/bin/bash
#
# Written by Martin Owens doctormo@gmail.com,
# License: Public Domain
# Revision: 2
#

VERSION=0.16
RELEASE=jaunty
PPA=ppa:doctormo/linberry-snapshot
DIR=linberry-$VERSION
GIT_DIR=linberry-git
#GIT_URL=git://repo.or.cz/linberry.git
GIT_URL=git://linberry.git.sourceforge.net/gitroot/linberry/linberry

[ -d $DIR ] && rm -fr $DIR
[ -d $GIT_DIR ] && (cd $GIT_DIR && git pull origin) || (git clone $GIT_URL $GIT_DIR)

# Get a copy of the code
rm linberry_$VERSION.orig.tar.gz
cd $GIT_DIR
git archive --prefix=$DIR/ master > ../linberry_$VERSION.orig.tar
cd ../
tar xvf linberry_$VERSION.orig.tar
gzip linberry_$VERSION.orig.tar

# Go into the code
cd $DIR

# Generate a new changelog file
rm debian/changelog
DATEVER=$VERSION-0git`date +%Y%m%d%S`
dch --create --package=linberry -v $DATEVER --distribution $RELEASE "Weekly GIT Build for $PPA"

# Build our debian source package
debuild -S

# Put our debian source package into launchpad
cd ../
dput $PPA linberry_${DATEVER}_source.changes

