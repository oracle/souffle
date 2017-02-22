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
    sudo apt-get -y install build-essential g++ automake autoconf bison flex openjdk-8-jdk lsb-release libtool clang-3.8
    clang++ --version
    sudo rm /usr/local/clang-3.5.0/bin/clang
    sudo rm /usr/local/clang-3.5.0/bin/clang++
    mkdir -p /home/travis/bin
    sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-3.8 100
    sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-3.8 100
    sudo ln -s /usr/bin/clang  /usr/local/clang-3.5.0/bin/clang
    sudo ln -s /usr/bin/clang++  /usr/local/clang-3.5.0/bin/clang++
#    CC=/usr/bin/clang
#    CXX=/usr/bin/clang++
    echo "PATH=$PATH"
    echo
    ls -latr ~/
    echo
    echo "~/.bash_profile"
    cat ~/.bash_profile
    echo
    echo "~/.bashrc"
    cat ~/.bashrc
    echo
    echo "~/.profile"
    cat ~/.profile
    echo
    export CC="clang-3.8"
    export CXX="clang++-3.8"
    which clang++
    clang++ --version
    /usr/bin/clang++ --version
    if [ "$TEST_FORMAT" == 1 ]
    then
        sudo apt-get -y install clang-format-3.8
        sudo update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-3.8 100
    else
        if [ "$MAKEPACKAGE" == 1 ]
        then
          sudo apt-get -y install debhelper devscripts
        fi
            if [[ "$SOUFFLE_CATEGORY" != *"Syntactic"* ]]
            then
              sudo add-apt-repository -y "deb http://ftp.us.debian.org/debian unstable main contrib non-free"
              sudo apt-get -qq update
              sudo apt-get -y --force-yes install libomp-dev
            fi
    fi
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
   brew install md5sha1sum bison libtool
   brew link bison --force
fi
