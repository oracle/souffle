#include "com_soufflelang_souffle_PrimData.h"
#include "handle.h"
#include <iostream>
#include <jni.h>
#include "conversion.h"
#include "RamData.h"

using namespace souffle;

void Java_com_soufflelang_souffle_PrimData_init(JNIEnv* env, jobject obj) {
    PrimData* data = getHandle<PrimData>(env, obj);
    delete data;
}

void Java_com_soufflelang_souffle_PrimData_release(JNIEnv* env, jobject obj) {
    PrimData* data = getHandle<PrimData>(env, obj);
    delete data;
}
