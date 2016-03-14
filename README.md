
# [Soufflé](https://souffle-lang.gitio.com)

[![Build Status](https://travis-ci.org/souffle-lang/souffle.svg?branch=master)](https://travis-ci.org/souffle-lang/souffle)

Soufflé is a translator of declarative Datalog programs into the C++ language.  Soufflé is used as a domain-specific language for static program analysis, over large code bases with millions of lines of code.  Soufflé aims at producing high-performance C++ code that can be compiled with the native compiler on the target machine.  

## Features of Soufflé

*   Efficient translation to parallel C++ of Datalog programs
*   Extended semantics of Pure Datalog, e.g., permitting unbounded recursions with numbers 
*   Simple component model for Datalog specifications 
*   Recursively defined record types for tuples 

## How to get Soufflé

Use git to obtain the source code of Soufflé. 

    $ git clone git://github.com/souffle-lang/souffle.git

## Soufflé home page

The URL of the Soufflé home page is:

http://souffle-lang.github.io

## Mailing list

There is no mailing list to talk about Soufflé at the moment. It will be established soon. 

## How to compile and install 

Follow the steps below to compile and install Soufflé on a UNIX system:


1.  G++ 4.8 or greater is recommended to compile Soufflé. 

2.  Run `sh ./bootstrap` to generate configure files 

3.  Run `./configure`

    For a MAC OS X computer, you require a bison version 3.0.2 or higher and a G++ compiler. 
    We recommend [brew](http://brew.sh), and run the following commands prior to executing `./configure`, 
     `brew update`                
     `brew install gcc --without-multilib`                
     `brew install bison`                
     `export CXX=/usr/local/bin/g++-5`                
     `export CXXFLAGS=-fopenmp`                
     `export SOUFFLECPP=/usr/local/bin/cpp-5`
    Note that the current CLANG installation on MAC OS X does not support OpenMP out of the box; the clang-cpp 
    cannot be used as a pre-processor for Souffle. Set
     `brew update`                
     `brew install bison`      
     `brew install gcc`      
     `brew install clang-omp`      
     `export CXXFLAGS=-fopenmp`                
     `export SOUFFLECPP=/usr/local/bin/cpp-5`

4.  Run `make` to build the executable of Soufflé

5.  Test the executable with `make check` to check whether the compilation of Soufflé succeeded.

6.  Run `make install`

    This command will create the directories and install files in `${DESTDIR}${prefix}`

## License

See [LICENSE](https://github.com/souffle-lang/souffle/blob/master/LICENSE).
