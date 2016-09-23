package com.soufflelang.souffle;
import com.soufflelang.souffle.*;

import java.util.List;

public class PrimData {
  public PrimData() { 
    init();
  }

  public PrimData(long d) {
    nativeHandle = d;
  }

  public native void init();
  public native void release();
  private long nativeHandle;

}
