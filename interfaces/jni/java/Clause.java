package com.soufflelang.souffle;
import com.soufflelang.souffle.*;

public class Clause {
    public Clause() {
      init();
    }

    private native void init();
    public native void setHead(Atom a);
    public native void addToBody(Literal l);
 
    private long nativeHandle;
}
