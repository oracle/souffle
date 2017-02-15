#include<jni.h>
#include "Jni_Const.h"
#include "handle.h"
#include "AstRelation.h"

using namespace souffle;

void Java_com_soufflelang_souffle_Const_init(JNIEnv* env, jobject obj, jlong num) {
    AstNumberConstant* c = new AstNumberConstant((int32_t)num);
    setHandle(env, obj, c);
}
