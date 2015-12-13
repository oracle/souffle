# Examples

## Simple Typed VarPointsTo

The following example encodes the most simple version of a var-points-to analysis.
```
// Encoded code fragment:
//
//  v1 = h1();
//  v2 = h2();
//  v1 = v2;
//  v3 = h3();
//  v1.f = v3;
//  v4 = v1.f;
//

.type var
.type obj
.type field

// -- inputs --
.decl assign( a:var, b:var )
.decl new( v:var, o:obj )
.decl ld( a:var, b:var, f:field )
.decl st( a:var, f:field, b:var )


// -- facts --

assign("v1","v2").

new("v1","h1").
new("v2","h2").
new("v3","h3").

st("v1","f","v3").
ld("v4","v1","f").


// -- analysis --

.decl alias( a:var, b:var ) output
alias(X,X) :- assign(X,_).
alias(X,X) :- assign(_,X).
alias(X,Y) :- assign(X,Y).
alias(X,Y) :- ld(X,A,F), alias(A,B), st(B,F,Y).

.decl pointsTo( a:var, o:obj ) output
pointsTo(X,Y) :- new(X,Y).
pointsTo(X,Y) :- alias(X,Z), pointsTo(Z,Y).
``` 
The example starts by declaring types for variables, objects and fields. Based on those, four input relations `assign`, `new`, `ld` and `st` are declared and filled with data corresponding to the small code snippet outlined in the comment above.

The analysis itself is broken up in two parts: 
* computation of aliases
* computation of the var-points-to relation based on aliases

Note that in particular for the last rule of the `alias` relation the utilization of typed attributes ensures that connections between attributes are consistently established. Problems caused by e.g. getting the wrong order of parameters can be effectively prevented.


## DefUse Chains with Composed Types
The following example utilizes a composed type to model a type hierarchy for instructions.
```
.type Var
.type Read
.type Write
.type Jump

.type Instr = Read | Write | Jump

// -- facts --
.decl read( i : Read, x : Var )
.decl write( w : Write, x : Var )
.decl succ( a : Instr, b : Instr )

read("r1","v1").
read("r2","v1").
read("r3","v2").
write("w1","v1").
write("w2","v2").
write("w3","v2").

succ("w1","o1").
succ("o1","r1").
succ("o1","r2").
succ("r2","r3").
succ("r3","w2").

// -- analysis --
.decl flow( a : Instr, b : Instr )
flow(X,Y) :- succ(X,Y).
flow(X,Z) :- flow(X,Y), flow(Y,Z).

.decl defUse( w : Write , r : Read ) output
defUse(W,R) :- write(W,X), flow(W,R), read(R,X).
```
In this example an instruction is either a read operation, a write operation or a jump instruction. However, to model the control flow through instructions, the `flow` relation needs to be able to cover any of those. 

To model this situation, the union type `Instr` is introduced and utilized as shown above.

## Context Sensitive Flow Graph
The following example demonstrates one way of integrating context information into a control flow graph.
```
.type Instr
.type Context

.decl succ( i1:Instr, c1:Context, i2:Instr, c2:Context )

succ( "w1", "c1" , "w2" , "c1" ).
succ( "w2", "c1" , "r1" , "c1" ).
succ( "r1", "c1" , "r2" , "c1" ).

succ( "w1", "c2" , "w2" , "c2" ).
succ( "w2", "c2" , "r1" , "c2" ).
succ( "r1", "c2" , "r2" , "c2" ).

.decl flow ( i1:Instr, c1:Context, i2:Instr, c2:Context )
flow(I1,C1,I2,C2) :- succ(I1,C1,I2,C2).
flow(I1,C1,I3,C3) :- flow(I1,C1,I2,C2), flow(I2,C2,I3,C3).

.decl res( a : symbol ) output
res("OK")  :- flow("w1","c1","r2","c1").
res("ERR") :- flow("w1","c1","r2","c2").
```
In this example the `flow` relation describes a graph where each node consists of a pair of an instruction and some (abstract) context. Although correct, the increased number of attributes causes larger code bases, and thus an increased risk of typos leading to hard-to-identify bugs. 

The fact that each node is represented by a pair of elements can be made explicit utilizing records as demonstrated next.

### Context Sensitive Flow Graph with Records
The following example is a refactored version of the context sensitive flow graph example above.
```
.type Instr
.type Context
.type ProgPoint = [
    i : Instr,
    c : Context
]

.decl succ( a : ProgPoint , b : ProgPoint )

succ( [ "w1", "c1" ] , [ "w2" , "c1" ] ).
succ( [ "w2", "c1" ] , [ "r1" , "c1" ] ).
succ( [ "r1", "c1" ] , [ "r2" , "c1" ] ).

succ( [ "w1", "c2" ] , [ "w2" , "c2" ] ).
succ( [ "w2", "c2" ] , [ "r1" , "c2" ] ).
succ( [ "r1", "c2" ] , [ "r2" , "c2" ] ).

.decl flow ( a : ProgPoint , b : ProgPoint )
flow(a,b) :- succ(a,b).
flow(a,c) :- flow(a,b), flow(b,c).

.decl res( a : symbol ) output
res("OK")  :- flow(["w1","c1"],["r2","c1"]).
res("ERR") :- flow(["w1","c1"],["r2","c2"]).
```
The type `ProgPoint` (Program Point) aggregates an instruction and an (abstract) context into a new entity which is utilized as the node type of the flow graph. Note that in this version `flow` is a simpler, binary relation and typos mixing up instructions and contexts of different program points are effectively prevented.

Also, as we will see below, the `flow` relation could now be modeled utilizing a generalized graph component, thereby inheriting a library of derived relations.


## Sequences using Recursive Records
The following example demonstrates the utilization of recursive records for building sequences of strings over a given alphabet:
```
.type Letter
.type Seq = [ l : Letter, r : Seq ]

.decl letter( l : Letter )
letter("a").
letter("b").

.decl seq ( s : Seq )
seq(nil).
seq([l,s]) :- letter(l), seq(s), len(s,n), n<5.

.decl len ( s : Seq, n:number )
len(nil,0).
len(s,n+1) :- seq(s), s = [l,r], len(r,n).

.decl res( s : symbol ) output
res("-") :- seq(nil).
res("a") :- seq(["a", nil ]).
res("b") :- seq(["b", nil ]).
res("c") :- seq(["c", nil ]).
res("ab") :- seq(["a", ["b", nil ]]).
res("aba") :- seq(["a", ["b", ["a", nil ]]]).
res("abc") :- seq(["a", ["b", ["c", nil ]]]).
```
The `Seq` type is a recursive type where each instance is either `nil` or a pair of a `Letter` (head) and a tailing `Seq` r. 

The relation `seq` is defined to contain all sequences of length 5 or less over a given alphabet defined by the relation `letter`. The relation `len` is essentially a function assigning each sequence its length. 

Finally, the `res` relation illustrates how to create constant values for recursive record types.

## Component Inheritance
Components provide means within Souffle's Datalog to build modular queries fostering the reuse of code.
```
.type node

.comp DiGraph {
    .decl node(a:node)
    .decl edge(a:node,b:node)

    node(X) :- edge(X,_).
    node(X) :- edge(_,X).

    .decl reach(a:node,b:node)
    reach(X,Y) :- edge(X,Y).
    reach(X,Z) :- reach(X,Y),reach(Y,Z).

    .decl clique(a:node,b:node)
    clique(X,Y) :- reach(X,Y),reach(Y,X).
}

.comp Graph : DiGraph {
    edge(X,Y) :- edge(Y,X).
}

.init Net = Graph

Net.edge("A","B").
Net.edge("B","C").

.decl res(a:node,b:node) output
res(X,Y) :- Net.reach(X,Y).
```
The given example defines a component `DiGraph` comprising of four relations: `node`, `edge`, `reach` and `clique`. Furthermore, defining rules for those relations determining their mutual relation are established utilizing ordinary Datalog rules. 

To model a undirected graph, the definition of the `DiGraph` is extended by an additional rule making all edges reflexive. This is expressed by the derived component `Graph`, which inherits all the declarations and definitions of the `DiGraph` component and extends it by one additional rule. 

Component hierarchies may be arbitrarily deep. A component may also have multiple base types, as long as their declared relations do not cause conflicts (e.g. two relations exhibiting the same name but different attributes).

The `.init` directive instantiates components by creating a copy of all its contained definitions, including a leading prefix. For instance, in the given example, the component `Graph` is instantiated for the name `Net`. Thus, the relations `Net.node`, `Net.edge`, `Net.reach` and `Net.clique` will be declared and defined accordingly. Those relations may then be accessed by other rules.

Note that components may also be nested. Thus, components may initialize other components within their body.

## Component Parametrization
Frequently, components can be described in an abstract, generic way such that it can be utilized in a wider range of use cases. The following example extends the example above by adding a type parameter describing the node type for a graph:
```
.comp DiGraph<N> {
    .decl node(a:N)
    .decl edge(a:N,b:N)

    node(X) :- edge(X,_).
    node(X) :- edge(_,X).

    .decl reach(a:N,b:N)
    reach(X,Y) :- edge(X,Y).
    reach(X,Z) :- reach(X,Y),reach(Y,Z).

    .decl clique(a:N,b:N)
    clique(X,Y) :- reach(X,Y),reach(Y,X).
}

.comp Graph<N> : DiGraph<N> {
    edge(X,Y) :- edge(Y,X).
}


.init NetA = Graph<symbol>
.init NetB = Graph<number>

NetA.edge("A","B").
NetA.edge("B","C").

.decl resA(a:symbol,b:symbol) output
resA(X,Y) :- NetA.reach(X,Y).


NetB.edge(1,2).
NetB.edge(2,3).

.decl resB(a:number,b:number) output
resB(X,Y) :- NetB.reach(X,Y).
```
The type parameter `<N>` introduces an additional element that can be fixed during the instantiation of a component. This type may be an arbitrary structure, including unions, records and recursive records. As a result, a graph of numbers, a graph of symbols (or a graph of program points as mentioned in the control flow example above) can share the same implementation of the actual graph. 

## Component Parametrized With Another Component
In the following example, component `Reachability` is parametrized with a name of another component `T`. 
When instantiating `Reachability` one must provide actual component that will be used as `T`. 
Component `Reachability` can use any relations from the instantiated (but yet unknown) component `T`. 

```
.comp Reachability<T> {
    .init graph = T

    .decl reach(a:N,b:N)
    reach(X,Y) :- graph.edge(X,Y).
    reach(X,Z) :- reach(X,Y),graph.edge(Y,Z).
}

.comp Graph1 {
    .decl edge(u:number,v:number)
    edge(1,2).
    edge(2,3).
    edge(3,4).
}

.comp Graph2 {
    .decl edge(u:number,v:number)
    edge(1,2).
    edge(3,4).
}

.init reach1 = Reachability<Graph1>
.init reach2 = Reachability<Graph2>

.decl res1(a:number,b:number) output
res1(X,Y) :- reach1.reach(X,Y). // output (1,2), (2,3), (3,4), (1,3), (1,4), (2,4)

.decl res2(a:number,b:number) output
res2(X,Y) :- reach2.reach(X,Y). // output (1,2), (3,4)
```
