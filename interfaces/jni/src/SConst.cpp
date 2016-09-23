#include<jni.h>
#include "com_soufflelang_souffle_SConst.h"
#include "handle.h"
#include "AstRelation.h"

using namespace souffle;

void Java_com_soufflelang_souffle_SConst_init(JNIEnv* env, jobject obj, jstring str) {
    const char *cStr = env->GetStringUTFChars(str, NULL);
    if (NULL == cStr) return;

    SymbolTable* symb = new SymbolTable();
    //size_t v = symb->lookup(std::string(cStr).c_str());
    AstStringConstant* c = new AstStringConstant(*symb, cStr);
    setHandle(env, obj, c);
}
