package com.soufflelang.souffle;

import com.soufflelang.souffle.Var;
import com.soufflelang.souffle.Literal;

public class Atom implements Literal
{
    public Atom(String name) {
      init(name);
    }
    
    private native void init(String name);
    public native void addArgument(Arg a);

    private long nativeHandle;
}
