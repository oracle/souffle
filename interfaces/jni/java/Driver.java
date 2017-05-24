package com.soufflelang.souffle;
import com.soufflelang.souffle.*;

import java.util.ArrayList;
import java.util.Arrays;

public class Driver {

    private static void create1(Program p) {
        // .type Node
        // .decl edge (node1:Node, node2:Node) input
        // .decl path (node1:Node, node2:Node) output
        // path(X,Y) :- path(X,Z), edge(Z,Y).
        // path(X,Y) :- edge(X,Y).

        // .type Node
        String ty = "Node"; 
        PrimType typ = new PrimType(ty, false);
        p.addType(typ);

        // .decl edge (node1:Node, node2:Node) input
        Relation r1 = new Relation("edge");
        r1.addAttribute("node1", ty);
        r1.addAttribute("node2", ty);
        //r1.setAsData();
        p.addRelation(r1);

        // .decl path (node1:Node, node2:Node) output
        Relation r2 = new Relation("path");
        r2.addAttribute("node1", ty);
        r2.addAttribute("node2", ty);
        r2.setAsOutput();
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

    public static void main(String[] args) {
        Program p = new Program();
        Solver souffle = new Solver();
        //Config config;

        //boolean toCompile = false;
        //ArrayList<ArrayList<String> > data = new ArrayList();
        //Map<String, ArrayList<ArrayList<String> > > relMaps = new Map();
        //relMap.insert("", data);
        Data data = new Data();
        data.addRelationTuple("edge", Arrays.asList(new String[]{"A", "B"}));
        data.addRelationTuple("edge", Arrays.asList(new String[]{"B", "C"}));
        data.addRelationTuple("edge", Arrays.asList(new String[]{"C", "D"}));
        data.addRelationTuple("edge", Arrays.asList(new String[]{"D", "E"}));
        data.addRelationTuple("egde", Arrays.asList(new String[]{"E", "F"}));
        data.addRelationTuple("edge", Arrays.asList(new String[]{"F", "A"}));

        create1(p);
        souffle.parse(p);
        //Result r = souffle.execute(data);
        System.out.println("got result \n");
        //r.print();
        // e.setInputData();
        //Results r = e.execute();
        //r.print();

        //if (souffle.init() != true) {
        //    println("error");
        // }

        //Results r = souffle.execute(data, toCompile);
        //Results.printAll();
    }

    static {
        System.load("/home/local/ANT/psubotic/Work/psubotic-Souffle/souffle/src/javaInterface/libdriver.so");
    }

}


