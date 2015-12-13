# Getting Started with SouffleProf

SouffleProf is a profiler for the Souffle Datalog Engine. It provides detailed information
about the performance of programs that Souffle runs. The profile log that Souffle produces 
is used as an input for SouffleProf. A textual user interface (TUI) is provided. The profiler can either run in an offline 
mode, i.e., the Souffle engine has finished its computation and written the profile information
to a log file, or in a live-mode. The live mode is used for looking at partial profile information. 

### Starting SouffleProf

To start the profiler, run the script souffle-profiler. The detailed usage instructions are as follows:

```
usage: souffle-profiler [-f <file> [-c <command>] [-l]] [-h] [-v]
-c <command>   run the given command on a log file
-f <file>      load the given log file
-h,--help      print this message
-l             run in live mode
```

Examples:
To start in the TUI mode in offline mode.
```
souffle-profiler -f <file>
```

To run a command without opening the TUI:
```
souffle-profiler -f <file> -c <command>  
```

# User Interface

Currently SouffleProf provides only a textual user interface. The
interface provides the following views:

* A top-level view of the performance totals of a program.
* A table view of all the performance characteristics of all Relations.
* A table view of all the performance characteristics of all Rules.
* A table view of all the performance characteristics of all the Rules of a Relation.
* A table view of all the performance characteristics of all the versions of a Rule.
* A chart view of runtime and total number of tuples per Iteration for a Relation.
* A chart view of runtime and total number of tuples per Iteration for a Rule.


The textual user interface is controlled by entering commmands. 
The following commands are available: 

```
rel                             - display relation table.
rel <relation id/name           - display all rules of given relation.
rul                             - display rule table
rul <rule id>                   - display all version of given rule.
rul id                          - display all rules names and ids.
rul id <rule id>                - display the rule name for the given rule id.
sort <col name>                 - sorts by given column.
graph <relation id> <type>      - graph the relation by type(tot_t/merge_t/tuples).
graph <rule id> <type>          - graph the rule by type(tot_t/tuples).
graph ver <rule id> <type>      - graph the rule versions by type(tot_t/tuples).
top                             - display top-level summary of program run.
load <filename>                 - load the given new log file.
open                            - list stored program run log files.
open <filename>                 - open the given stored log file.
save <filename>                 - store a log file.
stop                            - stop running live.
help                            - print this
q                               - quit
```

### Table Relation / Rule View

![rule](images/rule.png)

The table views show the performance data for each Rule/Relation. Relation and Rules can 
be referred to by their ID in commands. Relations can be referred to by their name and tab 
completion is available. Column TOT_R refers to the total runtime of a relation/rule, column 
NREC_T refers to the non-recursive portion of the runtime, column REC_T refers to the recursive 
portion of the runtime, column MERGE_T refers to the copy overheads related to the 
semi-naive evaluation updating the delta knowledge for the next iteration. The column
TUPLES, ID, and NAME, refers the the number of tuples produced, identifier of the relation/rule, 
and the name, respectively. 

