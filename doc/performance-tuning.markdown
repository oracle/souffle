# Performance Tuning

## Best Practice

Determining a good execution schedule for a given rule requires some insight regarding the internal operation of the data log engine. In general, a rule like
```
C(X,Z) :- A(X,Y), B(Y,Z).
```
is translated into a loop nest similar to
```
for( e1 : A ) {
    for( e2 : B ) {
        if ( e1[1] == e2[0] ) {
            C.insert( e1[0], e2[1] );
        }
    }
} 
```
The cost of this loop is mainly dominated by the number of times the innermost loop body, comprising the condition and the insertion operation, needs to be evaluated. The objective of finding a good is to minimize the number of inner-most loop-body executions.

Obviously, a naive conversion like this would be not very efficient. It would require |A| * |B| iterations ofer the innermost loop body. Fortunately, the second for loop and the condition can be combined into
```
for( e1 : A ) {
    for( e2 : { e in B | e.[0] == e1.[1] } ) {
        C.insert( e1[0], e2[1] );
    }
} 
```
Now, to speed up the execution, an ordered index on B can utilized to more effectively enumerate all the entries in B where the first component is equivalent to the second component of the currently processed tuple in A. This way, the execution cost is reduced to |A| * |fanout of A in B| which in most cases is considerably smaller.

In an extreme case, e.g. given by
```
C(X,Y) :- A(X,Y), B(X,Y).
```
which would be naively converted to
```
for( e1 : A ) {
    for( e2 : B ) {
        if ( e1[0] == e2[0] && e1[1] == e2[1] ) {
            C.insert( e1[0], e1[1] );
        }
    }
} 
```
the operation is contracted to
```
for( e1 : A ) {
    if ( e1 in B ) {
        C.insert( e1[0], e2[1] );
    }
} 
```
eliminating an entire loop, significantly reducing the number of times the loop body is processed.

## Profiler

The performance impact of the rule order can be measured using the profiling tool of Souffle. The runtime of a rule, how may tuples are produced by the rule, and the performance behavior for each iteration of a recursively defined rule can be measured and visualized. In practice, only a few rules are performance critical and needed to be considered for performance tuning. 

More detailed description follows. First souffle is executed with profiler log option enabled on a given datalog specification and a set of input facts. Then the generated log file is opened with souffle profiler. As described in the user manual section of souffle profiler, the profiler can be made to list the rules in the descending order of the total time consumed by each rule of the given datalog specification. This list is very useful in the sense that one can compare the time taken by each rule with the number of tuples it generated. The rules which consume more time by generating less tuples are the candidates for optimization. Typically optimizing the top 10 rules in the list is sufficient.

A simple way to optimize a rule is to reorder the relations in its body based on the insight explained in the above section (Best Practices). Note that due to the semi-naive evaluation technique some rules are transformed into multiple rules. They are listed as versions of the rule. A manual query planner may be used to reorder predicates corresponding to a particular version of the rule. An example is given below.

```
A(x) :- B(x), C(x).

.plan 1: (2,1)
```
Assume that the relations B and C depend on A. Then the relations A, B and C are mutually recursive. Then the different version of the above rule will look like

```
Version 0: A(x) :- \Delta B(x), C(x)
Version 1: A(x) :- B(x), \Delta C(x)
```
\Delta B(x) includes only the new tuples of B generated in the previous stage of the semi-naive evaluation.

And the statement starting with .plan swaps the relations in the body of the Version 1 of the rule.
Souffle also provides an auto-scheduler which finds orders automatically. However, the auto-scheduler is still experimental and does not produce high-quality orders. Note that to fully understand the optimisations a good understanding of the semi-naive algorithm is required.