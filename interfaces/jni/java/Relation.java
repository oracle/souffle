package com.soufflelang.souffle;

public class Relation {
  public Relation(String name) {
    init(name);
  }

  private native void init(String name);
  public native void addAttribute(String a, String b);
  public native void setAsInputData();
  public native void setAsOutputData();
  public native void setAsInput();
  public native void setAsOutput();

  private long nativeHandle;
}
