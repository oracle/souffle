package com.soufflelang.souffle;

import com.soufflelang.souffle.Atom;

public class Negation implements Literal
{
    public Negation(Atom atom) {
      init(atom);
    }
    
    private native void init(Atom atom);
    private long nativeHandle;
}
