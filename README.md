# [Soufflé](https://souffle-lang.gitio.com)

Soufflé is a translator of declarative Datalog programs into the C++ language.  Soufflé is used as a domain-specific language for static program analysis, over large code bases with millions of lines of code.  Soufflé aims at producing high-performance C++ code that can be compiled with the native compiler on the target machine.  

[travis]: https://travis-ci.org/souffle-lang/souffle

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

http://www.souffle-lang.gitio.com/

## Mailing list

There is no mailing list to talk about Soufflé at the moment. It will be established soon. 

## How to compile and install 

Follow the steps below to compile and install Soufflé on a UNIX system:


1.  G++ 4.8 or greater is required to compile Soufflé. Currently, CLANG++ is not supported. 

2.  Run `sh ./bootstrap` to generate configure files 

3.  Run `./configure`

    For a G++ installation on MAC OS X you need to specify the following flags before invoking `./configure`:

     `export CXX=/usr/local/bin/g++-5`                
     `export CXXFLAGS=-fopenmp`                
     `export SOUFFLECPP=/usr/local/bin/cpp-5`                

4.  Run `make` to build the executable of Soufflé

5.  Test the executable with `make check` to check whether the compilation of Soufflé succeeded.

6.  Run `make install`

    This command will create the directories and install files in `${DESTDIR}${prefix}`

## License

See [LICENSE](https://github.com/souffle-lang/souffle/blob/master/LICENSE).
