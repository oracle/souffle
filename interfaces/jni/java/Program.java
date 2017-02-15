package com.soufflelang.souffle;

import com.soufflelang.souffle.*;


public class Program {
  public Program() {
    init();
  }

  private native void init();
  public native void addRelation(Relation relation);
  public native void addType(Type type);
  public native void addClause(Clause clause);
  public native String print();
  public native void merge(Program p);
  public native SConst makeSConstant(String val);

  //native void addComponent(Component comp); 
  //native void addInstantiation(Instantiation inst);

  private long nativeHandle;

  public void getExample1(Program p) {
        // .type Node
        String ty = "Node"; 
        PrimType typ = new PrimType(ty, false);
        p.addType(typ);

        // .decl edge (node1:Node, node2:Node) input
        Relation r1 = new Relation("edge");
        r1.addAttribute("node1", ty);
        r1.addAttribute("node2", ty);
        r1.setAsInputData();
        p.addRelation(r1);

        // .decl path (node1:Node, node2:Node) output
        Relation r2 = new Relation("path");
        r2.addAttribute("node1", ty);
        r2.addAttribute("node2", ty);
        r2.setAsOutputData();
        p.addRelation(r2);

        // path(X,Y) :- path(X,Z), edge(Z,Y).
        Clause c1 = new Clause();
        Atom path = new Atom("path");
        path.addArgument(new Var("X"));
        path.addArgument(new Var("Y"));
        c1.setHead(path);

        Atom path2 = new Atom("path");
        path2.addArgument(new Var("X"));
        path2.addArgument(new Var("Z"));

        Atom edge = new Atom("edge");
        edge.addArgument(new Var("Z"));
        edge.addArgument(new Var("Y"));

        c1.addToBody(path2);
        c1.addToBody(edge);

        p.addClause(c1);

        Clause c2 = new Clause();
        Atom path3 = new Atom("path");
        path3.addArgument(new Var("X"));
        path3.addArgument(new Var("Y"));
        c2.setHead(path3);

        Atom edge2 = new Atom("edge");
        edge2.addArgument(new Var("X"));
        edge2.addArgument(new Var("Y"));
        c2.addToBody(edge2);

        p.addClause(c2);
  }


  public void getExample2(Program p) {
        // .type Node
        String ty = "Node"; 
        PrimType typ = new PrimType(ty, false);
        p.addType(typ);

        // .decl edge (node1:Node, node2:Node) input
        Relation r1 = new Relation("path");
        r1.addAttribute("node1", ty);
        r1.addAttribute("node2", ty);
        r1.setAsInputData();
        p.addRelation(r1);

        // .decl path (node1:Node, node2:Node) output
        Relation r2 = new Relation("path2");
        r2.addAttribute("node1", ty);
        r2.addAttribute("node2", ty);
        r2.setAsOutputData();
        p.addRelation(r2);

        // path2(X,Y) :- path(X,Y)
        Clause c1 = new Clause();
        Atom e2 = new Atom("path2");
        e2.addArgument(new Var("X"));
        e2.addArgument(new Var("X"));
        c1.setHead(e2);

        Atom p2 = new Atom("path");
        p2.addArgument(new Var("X"));
        p2.addArgument(new Var("Y"));
        c1.addToBody(p2);
        p.addClause(c1);
  }


  public void getExample3(Program p) {
        // .type Node
        String ty = "Node"; 
        PrimType typ = new PrimType(ty, false);
        p.addType(typ);

        // .decl edge (node1:Node, node2:Node) input
        Relation r1 = new Relation("atom__az");
        r1.addAttribute("node1", ty);
        r1.addAttribute("node2", ty);
        r1.addAttribute("node3", ty);
        r1.setAsInputData();
        p.addRelation(r1);

        // .decl edge_ok (node1:Node, node2:Node)
        Relation r2 = new Relation("az_ok");
        r2.addAttribute("node1", ty);
        p.addRelation(r2);

        // .decl query (node1:Node, node2:Node) output
        Relation r3 = new Relation("query0");
        r3.addAttribute("node1", ty);
        r3.setAsOutputData();
        p.addRelation(r3);

        // edge_ok(x, y) :- az_ok(x, "A", z).
        Clause c1 = new Clause();
        Atom azok1 = new Atom("az_ok");
        azok1.addArgument(new Var("X"));
        c1.setHead(azok1);

        Atom az1 = new Atom("atom__az");
        az1.addArgument(new Var("X"));
        az1.addArgument(p.makeSConstant("A"));
        az1.addArgument(new Var("Z"));
        c1.addToBody(az1);

        p.addClause(c1);

        // query(x) :- edge_ok(x).
        Clause c2 = new Clause();

        Atom query = new Atom("query0");
        query.addArgument(new Var("X"));
        c2.setHead(query);

        Atom azok2 = new Atom("az_ok");
        azok2.addArgument(new Var("X"));
        c2.addToBody(azok2);

        p.addClause(c2);
  }

}
