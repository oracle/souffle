# Building and Installation of Soufflé

## Software Requirements

For building and installing Souffle, following software must be installed:

* Make 

* Autoconf tools 

* GNU G++ supporting C++11 (from version 4.8 onwards) 

* Bison

* Flex

* DoxyGen

* GLPK

* Java JDK

### Ubuntu/Debian Packages

On a Ubuntu/Debian system, following command line would install the necessary packages:

```
sudo apt-get install build-essential g++ automake autoconf bison flex doxygen glpk-utils openjdk-7-jdk
```

Note that the Ubuntu/Debian version needs to be recent such that G++ 4.8 is part of the standard distribution.

### Software Requirements for the Bunch Cluster

On a machine in the Bunch cluster you need to build newer versions of some tools to use instead of the default installation.  The following versions appear to work: Autoconf 2.69, Automake 1.15, GLPK 4.55.  You can get suitable of the other tools needed by running:

```
module add doxygen/1.8.6 gcc/4.9.1 java/jdk/1.7.0/80 glpk/4.55 autoconf/2.6.9 automake/1.15 sqlite/3.8.0.2 bison/3.0.2
export JAVAPREFIX=/cm/shared/apps/java/jdk1.7.0_80
mkdir /scratch/<user>/.gradle;
export GRADLE_USER_HOME=/scratch/<user>/.gradle
```

## Build Soufflé

The Souffle project follows automake/autoconf conventions for configuring, installing and building software. To configure, build, test, and install the project, type:
```
 cd souffle
 sh ./bootstrap
 ./configure --prefix=<souffle-dir>
 make
 make check 
 make install
```
which stores executable, scripts, and header files in the directory of your choice denoted by ```<souffle-dir>```.  Use an absolute path for ```<souffle-dir>```.

By setting the path variable 
```
 PATH=$PATH:<souffle-dir>/bin
``` 
The commands ```souffle``` and ```souffle-profile``` are ready to use. 

# Running a simple example 

Create the following Datalog program and save it to `reachable.dl`.

```
// Type Node
.type Node

//
// Edge
//
.decl Edge        (n:Node, n:Node)

Edge("0","1").
Edge("1","2").
Edge("2","3").
Edge("1","4").

//
// Reachable 
//

.decl Reachable   (n:Node, n:Node) output

Reachable(x,y)  :- Edge(x,y).
Reachable(x,y)  :- Edge(x,z), Reachable(z,y).

```

This program computes a transitive closure of the input graph that is given by the set of edges (facts for relation `Edge`). The following command evaluates the program and prints the output via option `-D-`.  
```
souffle -D- reachable.dl
```

The command-line options for the tool are shown via command:

```
souffle -h
```

More Datalog examples can be found in the directory ```tests/evaluation/``` of the repository. 

The next topic is [how to compile/execute Datalog programs](compile_execute) with Soufflé.
