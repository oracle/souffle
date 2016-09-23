#include<jni.h>
#include "com_soufflelang_souffle_Var.h"
#include "handle.h"
#include "AstRelation.h"

using namespace souffle;

void Java_com_soufflelang_souffle_Var_init(JNIEnv* env, jobject obj, jstring str) {
    const char *cStr = env->GetStringUTFChars(str, NULL);
    if (NULL == cStr) return;

    AstVariable* var = new AstVariable(std::string(cStr));
    setHandle(env, obj, var);
}
