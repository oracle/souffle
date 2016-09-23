package com.soufflelang.souffle;

import com.soufflelang.souffle.Literal;

public class Constraint implements Literal
{
    public Constraint(String op, Arg lhs, Arg rhs) {
      init(op, lhs, rhs);
    }
    
    private native void init(String op, Arg lhs, Arg rhs);
   
    private long nativeHandle;
}
