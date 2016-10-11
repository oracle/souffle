package com.soufflelang.souffle;

public class Const extends Arg
{
  public Const(String num) {
    super(num);
    init(Long.decode(num));
  }

  private native void init(long num);
  private long nativeHandle;
}
