# Souffle's Arithmetic Built-Ins


* Souffle supports standard arithmetic operations **+**, **-**, **&#42;**, **&#47;**, **&#94;** and **&#37;**. Examples of this are given below.
```
.decl e(x:number, t:txt, y:number)
e(10 * 2,"10*2", 20) :- true(_). 
e(10 + 2,"10+2", 12) :- true(_). 
e(10 / 2,"10/2", 5) :- true(_).
e(10 ^ 2 , "10^2", 100) :- true(_).
e(10 % 3, "10%3", 1) :- true(_).
e(2^4%13 , "2^4%13",3) :- true(_).
```

* Souffle supports standard unary operation **-**.
```
e(-2*10,"-20", -20) :- true(_).
e(-2,"-2", -2) :- true(_).
e(--2,"--2", 2) :- true(_).
```

* Souffle supports standard binary operations **&#62;**, **&#60;**, **&#61;**, **&#33;&#61;**, **&#62;&#61;** and **&#60;&#61;**. Examples of this are given below.
```
A(a,c) :- a > c.
B(a,c) :- a < c.
C(a,c) :- a = c.
D(a,c) :- a != c.
E(a,c) :- a <= c.
F(a,c) :- a >= c.
```

* **&#36;** is used to generate unique random values to populate a table. It should be used with care as it may result in stepping outside the standard Datalog semantics.
```
.decl A (n:number)
.decl B (a:number, b:number)
.decl C (a:number, b:number) output
A(0).
A(i+1) :- A(i), i<1000.		
B($,i) :- A(i).		
C(i,j) :- B(c,i), B(c,j), i!=j.
```
The above example does not output anything.