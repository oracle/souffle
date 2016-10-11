package com.soufflelang.souffle;

import com.soufflelang.souffle.*;

public class Solver {
    public Solver(){
      init();
    }

    public native void init();
    public native void release();
    public native Executor parse(Program p);

    private long nativeHandle;
    private long returnHandle;
};
