package com.soufflelang.souffle;
import com.soufflelang.souffle.*;

public class PrimType extends Type {
  public PrimType(String name, boolean isNum) {
    super(name);
    init(name, isNum);
  }
 
  private native void init(String name, boolean b);

  private long nativeHandle;
}
