# Souffle's Built-in String Support

* **cat** is used to concatenate two strings together. It can be nested to concatenate more than two strings.
```
.type A
.decl Y (a:A, b:A) 
.decl Z (a:A, b:A, c:A) output
Y("a","b"). 
Y("c","d"). 
Z(a,b, cat(cat(a,b), a)) :- Y(a,b). 
```
The output would be:
```
a	b	aba
c	d	cdc
```

* **contains** is used to check if the latter string contains the former string.
```
.type String
.decl stringTable (t:String) 
.decl substringTable (t:String) 
.decl outputData  (substr:String, str:String) output
outputData(x,y) :- substringTable(x), stringTable(y), contains(x,y). 
stringTable("aaaa").
stringTable("abba").
stringTable("bcab").
stringTable("bdab").
substringTable("a").
substringTable("ab").
substringTable("cab").
```
The output would be:
```
a	aaaa
a	abba
a	bcab
a	bdab
ab	abba
ab	bcab
ab	bdab
cab	bcab
```

* **match** is used to check if the latter string matches a wildcard pattern specified in the former string.
```
.type String
.decl inputData   (t:String) 
.decl outputData  (t:String) output
outputData(x) :- inputData(x), match("a.*",x). 
inputData("aaaa").
inputData("abba").
inputData("bcab").
inputData("bdab").
```
The output would be:
```
aaaa
abba
```

* **ord** is used to evaluate the Unicode values of the corresponding character in the string. It is useful for comparing the strings based on the order of characters.
```
.type Name
.decl n ( x : Name )
n("Homer").
n("Marge").
n("Bart").
n("Lisa").
n("Maggie").
.decl r ( x : number ) output
r(1) :- n(x), n(y), ord(x) < ord(y).
r(2) :- n(x), n(y), ord(x) > ord(y).
```
The output would be:
```
1
2
```

* Equality operations (**=** and **!=**) are also available for string types, by performing an ordinal comparison.