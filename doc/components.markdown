# Components

## Definition & Instantiation
A component is a set of related relations. Component can be seen as a template. 
Once we instantiate a component, under the hood Souffle copies all the relations from 
its body and gives them unique names. However, in the source code, we can still refer to these 
new rules using the name of the component instance.

New component can be declared using the `.comp` keyword, the following block between `{` and `}` 
is the component body. In component body, we can declare rules and nested components.

```
.comp MyComponent {
   .decl TheAnswer(x:number)
   TheAnswer(42).
}
```

A component can be instantiated by using the `.init` keyword followed by unique 
name of the instance, equals sign and name of the component type.

```
.init myCompInstance1 = MyComponent
.decl Test(x:number)
Test(x) :- myCompoInstance1.TheAnswer(x).
```

Another instantiation of the same component will duplicate all the tables!

## Input rules
All the relations inside a component can be extended with new rules.

```
.init myComp = MyComponent
myComp.TheAnswer(33).

.decl Test(x:number)
Test(x) :- myCompoInstance1.TheAnswer(x). // output: 42, 33
```

One usage is to declare a relation inside a component, but leave it empty. 
Consumers of the component should provide the rules for the relation. 
This is one way of "passing parameters" into the component.

```
.comp Reachability {
  .decl edge(u:number,v:number)
  .decl reach(u:number,v:number)
  reach(u,v) :- edge(u,v).
  reach(u,v) :- reach(u,x), edge(x,v).
}

.init r = Reachability
r.edge(1, 2).
r.edge(2, 3).
r.edge(2, 4).
```

## Inheritance
One component can inherit from another. Inheritance in this case is purely 
injection of rules from the base component into its subcomponent. The syntax is as follows:

```
.comp Base {
  .decl TheAnswer(x:number)
  TheAnswer(x) :- 42.
}

.comp Child : Base {
  .decl WhatIsTheAnswer(n:number)
  WhatIsTheAnswer(n) :- TheAnswer(n).
}
```


## Type Parametrization
Components can be parametrized with types (including records) or other components in similar fashion 
as generics in Java or templates in C++. 

```
.comp Graph<TNode> {
  .decl edge(u:TNode, v:TNode)
  // ...
}
```

If the parameter is meant to be another component, it can be instantiated:

```
.comp Reachability<TGraph> {
  .init graph = TGraph
  // ...
  reach(u, v) :- graph.edge(u, v).
}
```

When initializing parametrized component, we must provide actual parameters:

```
.init g = Graph<number>
.init reach = Reachability<MyGraph>
```

Note that the following is not possible:

```
.init reach = Reachability<Graph<number>>
```

This limitation can be overcome with inheritance:

```
.comp NumberGraph : Graph<number> {}
.init reach = Reachability<NumberGraph>
```

Note that instantiation of `TGraph` inside `Reachability` 
will create new copies of `TGraph` relations (specifically relations 
of whatever is used as actual parameter for `TGraph`).

## Type Parametrization and Inheritance
Type parameters can passed around when inheriting. Example:

```
.comp A<T> { .... }
.comp B<K> : A<K> { ... }
```

Although it is not recommended, the type parameter can be used as the base class:

```
.comp A<T> : T { ... }
```

## Overridable Relations
If a relation, declared in a super component is overridable, it would be possible to override its associated rules in the sub-component while inheriting the rest of the relations from the super-component.
A relation in a sub-component with the same signature as a relation in the super component can override the super component's relation if it has `overridable` keyword. 

```
.comp A {
    .decl Rel(x:number) overridable
    Rel(1).
}
```
Since relation `Rel(x:number)` in the component `A` has `overridable` keyword, any sub-component of `A` can override this relation using `.override` keyword. 

```
.comp B : A {
    .override Rel
    Rel(2).
}


```
The override semantic replaces the clauses of all super-components by clauses of the sub-component. We omit the actual relation declaration in the sub-component because changing the signature of the relation would cause serious havoc with the use of the relation outside the scope of the sub-component.

As another example, we can use override semantics to implement a more precise version of an existing analysis by overriding some relations:

```
.comp AbstractPointsto{

    .decl  HeapAllocationMerge(heap,mergeHeap) overridable

    HeapAllocationMerge(heap,"<<string-constant>>") :-
           StringConstant(heap).

    ...

}


.comp PrecisePointsto : AbstractPointsto{

    .override HeapAllocationMerge

    HeapAllocationMerge(heap,"<<string-constant>>") :-
       StringConstant(heap),
       !ClassNameStringConstant(heap),
       !SimpleNameStringConstant(heap).
}

.init precise_pointsto = PrecisePointsto

```
In this example, PrecisePointsto inherits all the relations from AbstractPointsto, but only implements the HeapAllocationMerge relation differently. This feature avoids code duplications when we need several implementations of a generic analysis with small variations. 


## Examples
Examples on how to utilize components can be found [here](examples#component-inheritance).


