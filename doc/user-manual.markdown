# Datalog

Datalog is declarative programming language that has been
introduced as a query language for deductive databases in the late 70s
that has been successfully used in areas such as web semantics, 
program analysis, and security analysis. A full exposition is beyond the scope of this manual -- more details are available in the book Foundations of Databases by Abiteboul, Hull and Vianu or the [tutorial material] (http://blogs.evergreen.edu/sosw/files/2014/04/Green-Vol5-DBS-017.pdf)

Datalog uses first-order logic to express computations.
Programs consists of a set of Horn clauses.
A Horn clause is a disjunction of literals with at most one positive literal
that can be written in implication form as,

**u** ← (**p** ∧ **q** ∧ ... ∧ **t**)

that can be read as predicate **u** holds if the conditions
**p** ∧ **q** ∧ ... ∧ **t** hold.

Soufflé has only two types of Horn clauses:

* **facts**, with a single positive literal and no negated literals,

* **definite clauses**, that have a single positive literal and one or more negated literals.

Soufflé has no notion of goal clauses (i.e., disjunction of literals with no
single positive literal) due to the lack of an interactive mode.
Relations that are marked as **output relations**, contain the result
of the computation after executing the Datalog program.
In contrast to standard Datalog (i.e., finite domains, stratified negations, and totally ordered domains), Soufflé permits limited use of functors in
predicates and has extensions which make the language Turing equivalent.

## Relations

Soufflé requires to declare relations. A relation is a set
of ordered tuples (x<sub>1</sub>, ..., x<sub>k</sub>) where each
element x<sub>i</sub> is a member of a data domain denoted
by an attribute type. For example, the declaration

```
.decl A(x:number, y:number).
```

defines the relation A that may contain pairs of numbers. The
first attribute (sometimes referred by column) is named ``x``
and the second attribute is named ``y``. The names of the
attributes are meaningless in the actual Datalog program,
since only the argument positions in atoms.

### Attribute Types

To ease programming, we assume that the attributes of relations
may have different domains. A type system checks whether name
bindings in clauses are correct at compile time. The details of the
type system are covered below.

# Types

Souffle utilizes a typed Datalog dialect to conduct static checks enabling the early detection of errors in Datalog query specifications. Each attribute of involved relations has to be typed. Based on those, Souffle attempts to deduce a type for all terms within all the Horn clauses within a given Datalog program. In case no type can be deduced for some terms, a typing error is reported -- indicating a likely inconsistency in the query specification.

## Type Definitions 

There are two basic types in Souffle: (1) `symbol` and (2) `number`. Those types have the following properties:

* **Numbers** are signed integer numbers and are fixed to a fixed range. At the moment the range is fixed to the `int32_t` type covering the range [ -2^31 .. 2^31 -1 ]. However, the type could be altered by a compile time flags to any basic type that the C++ compiler supports. 

* **Symbols** are reference values that have a symbolic representation as well. They are currently represented as strings. Thus, string based operations, e.g. string concatenation, may introduce new values not being present in any input set to the stable result set.

Currently, no distinction between symbols representing entities and their string representation is made. As a consequence, any symbol may be interpreted as a string for string operations. This would, for instance, enable the creation of a new city "BrisbaneSydney" through an expression `cat(X,Y)` where the variables `X` and `Y` are bound correspondingly. It is the users responsibility to use `cat` only in sound contexts. Future development steps may resolve this shortcoming in the type system.

Based on those predefined types, user defined types may be specified. There are three ways of defining new types:

* **Primitive Types** are independent, user-defined types covering a subset of one of the predefined types (symbols or numbers). A primitive types are defined by constructs of the form:
```
.number_type weight       // creates a user defined number type
.symbol_type City         // creates a user defined symbol type
.type Village             // synonym to .symbol_type Village
```
Operators applicable to the corresponding base type (`number` or `symbol`) can also be applied on the corresponding user-defined primitive type. For instance, arithmetic operations can be applied on values of the `weight` type above -- thus, weights might be added or multiplied. 

* **Union Types** are combining multiple types into a new type. Union types are defined similar to
```
.type Place = Village | City
```
where `Place` on the left side is the name of the new type and on the right side there is a list of one or more types to be aggregated -- in this case `Village` and `City`. Any village or city value will  be a valid place.

The following sub-sections will provide more in-depth details on the semantic of types in Souffle's Datalog dialect.

# Relations

Relations are declared using the directive .decl followed by a parenthesis with its attribute names and types. For example,  

```
.decl A(x:number, y:number).
```

defines the relation A with two columns of type number. 

A relation can have additional qualifiers that indicate whether:
 * data should be read from a fact file with qualifier "input", 
 * the result of the relation is written to a file with qualifier "output", 
 * the number of tuples should be written at the end of execution with qualifier "printsize".

# Clauses

Clauses are conditional logic statements. A clause starts with a head followed by a body. For example, 
```
A(x,y) :- B(x,y). 
```
a pair (x,y) is in A, if it is in B. 

Clauses have qualifiers which direct the query planner for better query execution. The qualifier  
".strict" forces the query planner to use the order of the specified clause. The qualifer ".plan" 
permits the programmer to specify for each version of the clause an own schedule. Several versions 
of a clause may occur if the clause is evaluated in a fixpoint. 

# Negation

A clause of the form
```
CanRenovate(person, building) :- Owner(person, building), !Heritage(building).
```
expresses the rule that an owner can renovate a building with the condition the building is not classified as heritage. Thus the literal "Heritage(building)" is negated (via "!") in the body of the clause.

But negation needs to be used with care. For example,
```
A(x) :- ! B(x).
B(x) :- ! A(x).
```
is a circular definition. One cannot determine if anything belongs to the relation "A" without determining if it belongs to relation "B". But to determine if it is a "B" one needs to determine if the item belongs to "A". Such circular definitions are forbidden. Technically, rules involving negation must be stratifiable.

Negated literals do not bind variables. For example,
```
A(x,y) :- R(x), !S(y).
```
is not valid as the set of values that "y" can take is not clear. This can be rewritten as,
```
A(x,y) :- R(x), Scope(y), !S(y).
```
where the relation "Scope" defines the set of values that "y" can take.

# Comments and Pre-Processing
Souffle utilizes the same comment syntax as C/C++. Furthermore, all souffle programs are passed through the C pre-processor. As a consequence, e.g. `#include` pragmas may be utilized to organize Datalog input queries into several files and reuse common constructs within several programs. Also, constants may be defined utilizing the `#define` pragma.


# Advanced Topics
In addition to the topics covered on this page souffle offers a series of advanced features. Those are covered on their respective pages:
* [Type System](type-system)
* [Arithmetic Expressions](arithmetic-expressions)
* [String Expressions](string-expressions)
* [Records](records)
* [Aggregation](aggregates)
* [Components](components)
* [Performance Tuning](performance-tuning)
* [User Manual for Profiler](Profiler)



# Examples
A list of examples using the presented and advanced features can be found [here](examples).