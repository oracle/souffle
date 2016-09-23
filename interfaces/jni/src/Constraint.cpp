#include "com_soufflelang_souffle_Constraint.h"
#include "handle.h"
#include "conversion.h"
#include "AstLiteral.h"
#include "AstArgument.h"
#include <iostream>
#include <jni.h>


using namespace souffle;

void Java_com_soufflelang_souffle_Constraint_init(JNIEnv* env, jobject obj, jstring op, jobject lhs, jobject rhs) {
    AstArgument *lhsa = getHandle<AstArgument>(env, lhs);
    AstArgument *rhsa = getHandle<AstArgument>(env, rhs);
    std::string oper = std::string(env->GetStringUTFChars(op, 0));
    setHandle(env, obj, new AstConstraint(oper, std::unique_ptr<AstArgument>(lhsa), std::unique_ptr<AstArgument>(rhsa)));
}


