package com.soufflelang.souffle;

public class SConst extends Arg {
  public SConst(long nh) {
    nativeHandle = nh;
  }

  private long nativeHandle;
}
