#include<jni.h>
#include "com_soufflelang_souffle_BinOp.h"
#include "handle.h"
#include "AstLiteral.h"

using namespace souffle;

void Java_com_soufflelang_souffle_BinOp_init(JNIEnv* env, jobject obj, jstring str, jobject lhs, jobject rhs) {
    const char *cStr = env->GetStringUTFChars(str, NULL);
    if (NULL == cStr) return;
    AstBinaryFunctor* bop = new AstBinaryFunctor(getBinaryOpForSymbol(std::string(cStr)), 
                                                 std::unique_ptr<AstArgument>(getHandle<AstArgument>(env, lhs)), 
                                                 std::unique_ptr<AstArgument>(getHandle<AstArgument>(env, rhs))
                                                );
    setHandle(env, obj, bop);

}
