package com.soufflelang.souffle;

public class Var extends Arg
{
  public Var(String name) {
    init(name);
  }

  private native void init(String name);
  private long nativeHandle;
}
