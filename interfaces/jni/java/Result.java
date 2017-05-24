package com.soufflelang.souffle;

import java.util.Set;
import java.util.List;
import java.util.Arrays;
import java.util.ArrayList;

public class Result {

  public Result(long env) { 
    nativeHandle = env;
  }

  public native void release();
  public native PrimData getPrimData(String rname);
  public native List<List<String>> getRelationRows(String rname);
  public native List<String> getRelationNames();

  private long nativeHandle;
}
