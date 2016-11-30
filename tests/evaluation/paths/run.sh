#!/bin/bash

pushd ../../.. && make && popd && \
rm -f file* file*.cpp && \
./../../../src/souffle -Ffacts -D. -c $(basename $(pwd)).dl 1> /dev/null 2> $(basename $(pwd)).error
#valgrind ./file*
