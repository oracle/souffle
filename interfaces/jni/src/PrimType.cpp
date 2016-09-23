#include<jni.h>
#include "com_soufflelang_souffle_PrimType.h"
#include "handle.h"
#include "AstType.h"

using namespace souffle;

void Java_com_soufflelang_souffle_PrimType_init(JNIEnv* env, jobject obj, jstring str, jboolean b) {
    const char *cStr = env->GetStringUTFChars(str, NULL);
    if (NULL == cStr) return;
    AstPrimitiveType* ty = new AstPrimitiveType(AstTypeIdentifier(std::string(cStr)), (bool)b);
    setHandle(env, obj, ty);
}
