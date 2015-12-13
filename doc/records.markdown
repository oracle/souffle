# Records

* theory
* overview
* type system
* syntax
  * constructors
  * field projection
  * nil constant
* example
  * non-recursive example
  * recursive example

* **Record Types** are combining several values into a structured value similar to a `struct` in C. An example record definition is given by
```
.type Connection = [
	from : Place,
	to : Place
]
```
defining values of ordered pairs of places. Each record type enumerates a list of nested, named fields and their types. Records may be nested as in
```
.type Cargo = [
	flight : Connection,
	mass : weight
]
```
as well as recursive, as in
```
.type Path = [
	first : Connection,
	rest : Path
]
```
Thus, a record may contain (directly or indirectly) fields of its own type.