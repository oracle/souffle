# Aggregates

* theory - stratification
* list of aggregates
* examples

## Examples

The term 
```
min Z : a("A",Z).
```
computes the smallest Z such that the value ["A",Z] is present in relation `a`. If there is no such element, the result is unbound and the containing body will not be satisfied.

By using wild cards, as in
```
min Z : a(_,Z).
```
the overall smallest value of the second attribute can be obtained, independently of the first attribute. However, the operation will fail if `a` is empty. Finally, bound variables may be referenced in the body of the aggregation, as in
```
res(X,Y) :- a(X,_), Y = min Z : a(X,Z).
```
which maps each element of the first column of `a` to its minimal corresponding value in the second column.

Finally, more complex terms may be minimized for. For instance, in
```
min Z+Y : { a(A,Z), a(B,Y), A!=B }.
```
the sum of two values is minimized. The given term computes the smallest sum of values assigned to two different elements in `a`.


