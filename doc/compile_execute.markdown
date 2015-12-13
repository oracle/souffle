# Compiling & Executing Datalog Programs

**TODO**: Refine wording, draw flow diagrams as explanation for the various modes

Soufflé provides an interpreter, a compiler, and a feedback-directed compilation infrastructure for compiling and executing Datalog programs. Profile information can be generated and visualized with a [profiling tool](profiler). The various modi how to run Soufflé is determined by the argument parameters of the souffle command. 

## Input / Output
The Soufflé permits that facts can be sourced from tab-separated input files to separate the Datalog programs from their data. In Datalog literature the tab-separated input files can be seen as the extensional database (EDB) of the program. The input of a Datalog program are stored in form of fact files that are tab-separated files. The location of the input files are specified by the parameter ```-F <fact-dir>```, the files for the input relations are expected in director ```<fact-dir>```. If flag is not specified, it is assumed that the fact files are stored in the current directory. The filenames of the input files must coincide with the names of the input relations in the program. For example, the declaration 
```
.decl my_relation(a:number,b:number) input
```
defines an input relation ```my_relation``` with two number columns. Note that a relation becomes an input relation with the keyword ```input``` at the end of a declaration.  For the aforementioned example , souffle expects a tab-separated file ```my_relation.facts``` in either the current directory (if no ```-F``` flag was specified) or it expects the file ```my_relation.csv``` in the directory ```<fact-dir>``` with the option ```-F <fact-dir>```. Note that there is an exception if a relation is an input relation is declared in an instantiation of a [component](components). 

The output relations of a Datalog program are either written to standard output with the parameter ```-D-``` or to the output directory ```-D <output-dir>```. If no flag ```-D``` is specified, the output is omitted. For example, the relation  
```
.decl result(a:number,b:number,c:number) output
```
has three number columns that are written either to the file ```result.csv``` in the directory ```<output-dir>``` using the flag ```-D <output-dir>```  or to standard output using the flag ```-D-```. If the keyword ```printsize``` is used, the size of the relation is always print to standard output.
For example, the relation  
```
.decl result(a:number,b:number,c:number) printsize
```
is not printed to a file. Instead the number of tuples of relation ```result``` are written to standard output. 

## Interpreter

The interpreter is the default option when invoking the ```souffle``` as a command line tool. When souffle is invoked in interpreter mode, the parser translates the Datalog program to a RAM program, and executes the RAM program on-the-fly. For computational intensive Datalog programs, the interpretation is slower than the compilation to C++. However, the interpreter has no costs for compiling a RAM program to C++ and invoking the C++ compiler which is for larger programs expensive (in the order of minutes). 

## Compiler 

The compiler of souffle is enabled using the flag ```-c``` or the flag ```-o <exec>``` 
(or its long version ```--dl-program=<exec>```). The compiler translates a Datalog program to a C++ program that is compiled to an executable and executed. The performance of a compiled Datalog is superior to the interpreter, however, the compilation of the C++ program may take some time. 

The difference between the flag ```-c``` and ```-o``` (or its long version ```--dl-program```) is whether the program is compiled and immediately executed with the former option or whether an executable is generated with the latter option. If compiled with option ```-o <exec```, the executable is a standalone program whose options can be queried with flag ```-h```. The following message would be produced,

```
====================================================================
 Datalog Program: <source file>
 Usage: <exec> [OPTION]

 Options:
    -D <DIR>, --output=<DIR>     -- directory for output relations
                                    (default: <fact-dir>) 
    -F <DIR>, --facts=<DIR>      -- directory for fact files
                                    (default: <output-dir>) 
    -h                           -- Prints this help page.
--------------------------------------------------------------------
 Copyright (c) 2013-15, Oracle and/or its affiliates.
 All rights reserved.
===================================================================
```

The defaults are taken from the compiler invocation, which may be overwritten with user defined parameters of the standalone executable. Note that if the profiling option is enabled, the standalone executable has the additional option ```-p``` (see below). 

## Feedback-Directed Compilation

Soufflé has implemented feedback-directed compilation, i.e., the interpreter executes a Datalog program. Whilst executing the Datalog program on-the-fly, the runtime behavior (relation sizes, etc.) are collected and used for producing a compiled executable of the input program. The feedback-directed compilation is enabled using the flags
```
souffle --auto-schedule -o <exec> <prog>.dl
```
where ```<exec>``` is the executable generated by the input program ```<prog>.dl``` using feedback-directed compilation. 
At the moment the query planner is still very experimental and does not produce optimal query plans. Optimizing queries by hand give better performance. More details can be found in the section for [performance tuning](performance-tuning).  

## Profiling 
As a side-effect of execution, a profile log can be generated. The profile log can be visualized using the souffle-profiler. The option for enabling the profile log is option ```-p <log-file>``` that works for the interpreter as well as the compiler. The profiler is described in the [profiler](profiler) section. 

## Output to Sqlite Database
The relations computed by the Datalog program can be output to an sqlite database for offline querying.

After using Souffle to generate an executable (with the ```-o``` option), executing the generated program with the ```--output-db <db-file>``` option will cause the relations to be stored in the specified file.

If the program was compiled with the ```--debug``` option, then the contents of all relations will be stored in the database, otherwise, only output relations will be included.

The database schema consists of:
 - a symbol table, ```__SymbolTable``` (mapping symbol ids to their corresponding string)
 - a ```_<relation-name>``` table for each relation (in which symbols are referred to by their ids)
 - a ```<relation-name>``` view for each relation (in which symbols are resolved to their corresponding string)

Note that record types are not fully supported (they are only stored as their id, and their fields are not accessible).

## Miscellaneous 

Souffle provides various means to debug and visualize/read the input program as a strongly connected component graph, RAM program, formated program, and or optimized program. The options for enabling these features are listed below:
```
    --dl-file=<FILE>               Write datalog program to a file in pretty print format
    --opt-dl-file=<FILE>           Write optimized datalog program to a file in pretty print format
    --ram-file=<FILE>              Write RAM program as text file
    --scc-graph=<FILE>             Write SCC graph as graphviz-file
    --prec-graph=<FILE>            Write precedence graph as graphviz-file
    --schedule-report=<FILE>       Write scheduling report to file, - for std-out

    -v, --verbose                  Verbose output
```




