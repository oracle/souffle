# Souffle's Type System 



## Domains

The semantic of types is defined by their associated domains. Let T be a type and D(T) denote its associated domain. The domains are given by the following rules:

* D(number) = D(uint32_t) (or altered by compiler flag)
* D(symbol) is the set of all strings over the set of printable characters
* D(U) ⊂ D(number) if U is defined by `.number_type U`
* D(S) ⊂ D(symbol) if S is defined by `.symbol_type S`
* D( T1 | T2 | ... | Tn ) ⊇ D(T1) ∪ D(T2) ∪ ... ∪ D(Tn)
* D( [ f1 : T1 , f2 : T2 , ... , fn : Tn ) ⊇ D(T1) x D(T2) x ... x D(Tn)

Furthermore, for two primitive types U and S we have that

* D(U) ∩ D(S) = ∅

Thus, by definition, no two primitive user-defined types U and S may have common elements. Consider the following type definitions:
```
.number_type weight
.number_type length
```
For both types, weight and length, the value 30 is a valid value. However, an expression of value 30 and type `weight` is not considered equivalent to an expression of value 30 and type `length`. Both values may be considered to be tagged by their type and the type is essential for equivalence.

Consequently, with the Datalog program
```
.number_type weight
.number_type length

.decl A(w:weight)
.decl B(l:length)

A(X) :- B(X).
```
the trailing rule will cause a typing error since no value of the first attribute of B will ever be in the domain of the first attribute of A, since D(weight) ∩ D(length) = ∅.

This interaction between types is generalized by the sub-type relation to be covered next.


## Sub-typing

The sub-type relation among types provides a foundation for determining whether there might be values in the intersection between (potentially infinite) domains of types. Intuitively, a type U is a subtype of S, denoted by U <: S, if all elements in the domain of U are also to be found in the domain of S. The following rules define the sub-type relation:

The sub-type relation is reflexive and transitive:
* S <: S for all types S
* if S <: T and T <: U we have S <: U for all types S, T, and U

All primitive types are sub-types of their predefined base type:
* U <: number if U is defined by `.number_type U`
* U <: symbol if U is defined by `.symbol_type U`

Thus, every value of a primitive type, e.g. the information that a weight is 40, can 'decay' to an ordinary number without the implicit type-tag of being a weight.

Finally, for union types the following rules hold:
* U <: T1 | ... | Tn if there is a 1 ≤ i ≤ n such that U <: Ti
* T1 | ... | Tn <: U if for all 1 ≤ i ≤ n we have Ti <: U
* U1 | ... | Un <: T1 | ... | Tm  if for all 1 ≤ i ≤ n there is a 1 ≤ j ≤ m such that Ui <: Tj

The first two rules define the relation between union types and non-union types and the third rule defines the relation between two union types. Note that a union type consisting of a single type is a sub-type of this type as well as the other way around.

There are no sub-typing relations for record types. Each record type constitutes its own, distinct domain. Thus, no element of one domain may be found in the domain of any other type's domain.

## Pitfalls

As an example, consider the following code fragment:
```
.type even = number
.type odd = number
.decl A(x:even)
.decl B(x:odd)
A(X) :- B(X)
```
In this example, the types `even` and `odd` are defined as union types over the single type `number`. Thus, they are effectively equivalent and no typing error will result from the rule stated for relation `A`.

However, when the aim of defining the two types `even` and `odd` was to enforce that those are two disjunctive sets of integer values, the rule stated for `A` should trigger at least a warning since no `X` should be within both -- the `even` and `odd` domain.

The problem is caused by utilizing a degenerated version of a union for the definition of the new types. A more accurate modeling is shown here:
```
.number_type even
.number_type odd

.decl A(x:even)
.decl B(x:odd)
A(X) :- B(X)
```
In this version, `even` and `odd` are defined as two disjunctive sub-types of the number-type, each exhibiting its own domain of values. From this definition Souffle can deduce that the users intention was to define two disjunctive sets and will report an error for the rule in the last line.

The type system is designed to support the developer of a Datalog program in enforcing constraints on the 'dataflow' within the resulting program. However, to enable the system to do so, an accurate modeling of the categories of values in form of a type system is required -- as has been illustrated by the example above.

