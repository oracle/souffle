#!/bin/bash

# Enable debugging and logging of shell operations
# that are executed.
set -e
set -x

echo -n "Version: "
git describe --tags --abbrev=0 --always
changedLines() {
  range_start=-1
  
  for m in $(git diff -U0 HEAD~ $1|grep '^@@'|sed -r 's/^@@.* \+([0-9][0-9]*),?([0-9]*) .*$/_\1 _\2/'); do
    n=${m:1};
    if (( range_start > -1 )) ; then
      if [[ "$n" == "" ]]
      then
        echo -n " -lines=$range_start:$range_start"
      else
        if (( n > 0 )) ; then
            echo -n " -lines=$range_start:$((n + range_start))"
        fi
      fi
      range_start=-1
    else
      range_start=$n
    fi
  done
  
  if (( range_start > -1 )) ; then
    echo -n " -lines=$range_start:$range_start"
  fi
}

if [ "$TEST_FORMAT" == 1 ]
then
  clang-format-3.6 --version
  set +x
  for f in $(git diff --name-only --diff-filter=ACMRTUXB HEAD~); do
    if ! echo "$f" | egrep -q "[.](cpp|h)$"; then
      continue
    fi
    if ! echo "$f" | egrep -q "^src/"; then
      continue
    fi
    echo "Changed lines: $(changedLines $f)"
    echo "log: $(git log -n2)"
    d=$(diff -u0 "$f" <(clang-format-3.6 -style=file $(changedLines $f) "$f")) || true
    if [ -n "$d" ]; then
      echo "!!! $f not compliant to coding style, here is a suggested fix:"
      echo "$d"
      fail=1
    fi
  done

  if [ "$fail" == 1 ]
  then
    exit 1
  else
    exit 0
  fi
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


