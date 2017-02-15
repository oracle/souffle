#!/bin/bash

# Enable debugging and logging of shell operations
# that are executed.
set -e
set -x

echo -n "Version: "
git describe --tags --abbrev=0 --always
if [ "$TEST_FORMAT" == 1 ]
then
  /usr/bin/clang-format --version
  $(dirname $0)/checkStyle.sh HEAD~
  exit 0
fi

# create configure files
./bootstrap

# configure project
if [ "$MAKEPACKAGE" == 1 ]
then
  ./configure --enable-host-packaging
else
  ./configure
fi

# create deployment directory
mkdir deploy

###############
# Linux Build #
###############
if [ $TRAVIS_OS_NAME == linux ]
then
  if [ "$MAKEPACKAGE" == 1 ]
  then
    make -j2 package
    # compute md5 for package &
    # copy files to deploy directory
    for f in packaging/*.deb
    do
      pkg=`basename $f .deb`
      src="packaging/$pkg.deb"
      dest="deploy/$pkg.deb"
      cp $src $dest
      md5sum <$src >deploy/$pkg.md5
    done
    # show contents of deployment
    ls deploy/*
  else
    make -j2
    if [[ "$SOUFFLE_CATEGORY" != *"Unit"* ]]
    then
      cd tests
    fi
    TESTSUITEFLAGS="-j2 $TESTRANGE" make check
  fi
fi

############
# MAC OS X #
############

if [ $TRAVIS_OS_NAME == osx ]
then
  if [ "$MAKEPACKAGE" == 1 ]
  then
    make -j2 package
    # compute md5 for package &
    # copy files to deploy directory
    for f in *.pkg
    do
      pkg=`basename $f .pkg`
      src="$pkg.pkg"
      dest="deploy/$pkg.pkg"
      cp $src $dest
      md5sum <$src >deploy/$pkg.md5
    done
    # show contents of deployment
    ls deploy/*
  else
    make -j2
    if [[ "$SOUFFLE_CATEGORY" != *"Unit"* ]]
    then
      cd tests
    fi
    TESTSUITEFLAGS="-j2 $TESTRANGE" make check
  fi
fi


