package com.soufflelang.souffle;
import com.soufflelang.souffle.*;

import java.util.List;

public class Data {

  public Data(long nh){
    nativeHandle = nh;
  }

  public Data() {
    init();
  }

  private native void init();
  public native void release();
  public native void print();
  public native Data merge(Data d);
  public native void addRelationTuple(String name, List<String> data);
  public native void addRelationData(String name, PrimData data);

  private long nativeHandle;
}
