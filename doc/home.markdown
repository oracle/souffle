# Soufflé 
![souffle](images/souffle.png)

The Soufflé project designs and implements novel compilation techniques for the efficient translation of Datalog programs to C++. The aim of the Souffle Datalog compiler is to use it as a domain-specific language tool for static program analysis including points-to, taint, constant-propagation, and security analyses for large-scale code bases with million lines of code. Soufflé provides the ability to rapid prototype
and make deep design space explorations possible for the tool designers writing static
program analyses with the aim to increase the productivity.

One of the major challenges in a declarative programming approach like Datalog is to make
the execution of static program analyses written in Datalog scalable for large code bases.
Hence, new high-performance execution models are required that go beyond state of-the art
techniques. Soufflé differs from existing Datalog implementations. Soufflé aims to be a
computational machinery for static program analyses solely without the database
management aspects. For this reason, new compilation techniques are investigated such as improved semi-naive evaluation, staged-compilation with a new abstract machine, partial evaluation, and parallelization for achieving high performance.

This research effort differs from existing Datalog research: In this research project, the Soufflé datalog compiler aims to be a computational machinery rather than a front-end language for a relational database. The major research questions in the Soufflé project are centered around:
* How to translate declarative rules to imperative programs efficiently on modern
computer hardware including multi-core computers and heterogeneous systems to obtain peak performance
(SIMDs, GPGPUs, clusters, etc.)?
* What domain specific language extensions of Datalog are necessary in Soufflé
expressing static program analyses and its development more effectively?


## People 

* Current Contributors
   * Bernhard Scholz
   * Nicholas Allen


* Past Contributors
   * Herbert Jordan
   * Pavle Subotic
   * Raihan Amod
   * Till Westmann



 

## Datalog Translation

We introduce a novel compilation technique that translates a Datalog program to a native
C++ program. The relational algebra operations for the execution of Datalog programs
are performed on highly optimized C++ data structures in memory, since for static program
analysis no persistent storage or concurrency control is required, and hence the compilation
to pure C++ yields high performance.
To obtain a highly optimized C++ program, we employ staged compilation: a Datalog
program is translated to an abstract machine using a semi-naive evaluation scheme, and then
further translated to a C++ program. The abstract machine is a relational algebra machine,
that can express the imperative execution of relational algebra operation and
fixed-point computations for the evaluation of Datalog programs. By lowering the
Datalog program to C++, computations can be moved from runtime to compile-time. 
This new compilation technique overcomes performance bottlenecks observed in traditional
Datalog implementations that are (1) implemented as an interpreter
and (2) perform the relational algebra operations on disk using a relational database.

![souffle-compiler](images/souffle-compiler.png)

### [How to Build Souffle](build)

### [How to Compile/Execute Datalog Programs](compile_execute)

### [User Manual](user-manual) 

### [Design of Souffle](Internals)

### [Research & Development](Ideas)

#### Why the name Souffle?
<sub>
Soufflé  is short for Systematic, Ontological, Undiscovered Fact Finding Logic Engine. The EDB represents the
uncooked Soufflé  and the IDB causes the Soufflé  to rise, i.e., monotonically increasing knowledge. When it stops rising and a fixed-point is reached, the result is a puffed-up ready-to-eat Soufflé. Big thanks to Nicholas Allen and Diane Corney for finding a translation :+1:.
</sub>

