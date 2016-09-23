package com.soufflelang.souffle;

public class SConst extends Arg
{
  public SConst(String value) {
    super(value);
    init(value);
  }

  private native void init(String value);
  private long nativeHandle;
}
