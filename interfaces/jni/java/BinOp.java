package com.soufflelang.souffle;

public class BinOp extends Arg
{
  public BinOp(String name, Arg lhs, Arg rhs) {
    super(name);
    init(name, lhs, rhs);
  }

  private native void init(String name, Arg lhs, Arg rhs);
  private long nativeHandle;
}
