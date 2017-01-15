#!/bin/bash

# Enable debugging and logging of shell operations
# that are executed.
set -e
set -x

echo -n "Version: "
git describe --tags --abbrev=0 --always

# create configure files
./bootstrap

# configure project
./configure --enable-host-packaging

# create deployment directory
mkdir deploy

###############
# Linux Build #
###############
if [ $TRAVIS_OS_NAME == linux ]
then
  # If c++ compiler is g++ and MAKEPACKAGE == 1, build Debian package.
  # Otherwise, just check whether latest check-in works
  if [ $CXX == g++ ]
  then
    if [ $MAKEPACKAGE == 1 ]
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
      TESTSUITEFLAGS="-j3 $TESTRANGE" make check
    fi 
  else
    make -j2
    TESTSUITEFLAGS="-j3 $TESTRANGE" make check
  fi
fi

############
# MAC OS X #
############

if [ $TRAVIS_OS_NAME == osx ]
then
  if [ $MAKEPACKAGE == 1 ]
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
    TESTSUITEFLAGS="-j3 $TESTRANGE" make check
  fi
fi
