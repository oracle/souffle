#include<jni.h>
#include "com_soufflelang_souffle_Atom.h"
#include "handle.h"
#include "AstLiteral.h"
#include "AstRelationIdentifier.h"

using namespace souffle;

void Java_com_soufflelang_souffle_Atom_init(JNIEnv* env, jobject obj, jstring str) {
    const char *cStr = env->GetStringUTFChars(str, NULL);
    if (NULL == cStr) return;

    AstAtom* atom = new AstAtom(AstRelationIdentifier(std::string(cStr)));
    
    setHandle(env, obj, atom);
}

// TODO: release

void Java_com_soufflelang_souffle_Atom_addArgument(JNIEnv* env, jobject obj1, jobject obj2) {
    AstAtom *l = getHandle<AstAtom>(env, obj1);
    AstArgument *a = getHandle<AstArgument>(env, obj2);
    l->addArgument(std::unique_ptr<AstArgument>(a));
}
