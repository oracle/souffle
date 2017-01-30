#!/bin/bash

# Enable debugging and logging of shell operations
# that are executed.
set -e
set -x

#########
# Linux #
#########

# Install requirements of Linux
if [ $TRAVIS_OS_NAME == linux ]
then
    sudo apt-get -qq update # TODO comment this out if the apt repo is up-to-date
    if [ "$MAKEPACKAGE" == 1 ]
    then
      sudo apt-get -y install debhelper devscripts
    fi
    sudo apt-get -y install build-essential g++ automake autoconf bison flex openjdk-8-jdk lsb-release libtool libedit-dev
    # The following lines are hacked because travis stopped working around 5/12/16, if you can remove them and travis still works, then great
#    source /opt/jdk_switcher/jdk_switcher.sh
#    jdk_switcher use openjdk7
fi

############
# MAC OS X #
############

# Install requirements of MAC OS X
if [ $TRAVIS_OS_NAME == osx ]
then
   brew update
   brew install md5sha1sum bison libtool homebrew/dupes/libedit
   brew link bison --force
fi
