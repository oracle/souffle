#include "com_soufflelang_souffle_Negation.h"
#include "AstLiteral.h"
#include "handle.h"
#include "conversion.h"
#include <iostream>
#include <jni.h>

using namespace souffle;

void Java_com_soufflelang_souffle_Negation_init(JNIEnv* env, jobject obj1, jobject obj2) {
    AstAtom *at = getHandle<AstAtom>(env, obj2);
    setHandle(env, obj1, new AstNegation(std::unique_ptr<AstAtom>(at)));
}


