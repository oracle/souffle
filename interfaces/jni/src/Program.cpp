#include<jni.h>
#include "Jni_Program.h"
#include "handle.h"

#include "AstBuilder.h"

using namespace souffle;

void Java_com_soufflelang_souffle_Program_init(JNIEnv *env, jobject obj) {
    LOG(INFO) ENTERJNI("init");
    AstBuilder* builder = new AstBuilder();
    LOG(MEM) PRE << "Creating AstBuilder object " << builder << "\n";
    setHandle(env, obj, builder);
    LOG(MEM) PRE << "Setting AstBuilder to handle " << builder << "\n";
    LOG(INFO) LEAVEJNI;
}

void Java_com_soufflelang_souffle_Program_addRelation(JNIEnv *env, jobject obj1, jobject obj2) {
    LOG(INFO) ENTERJNI("addRelation");
    AstBuilder *prog = getHandle<AstBuilder>(env, obj1);
    AstRelation *rel = getHandle<AstRelation>(env, obj2);
    prog->addRelation(rel);
    LOG(INFO) LEAVEJNI;
}

void Java_com_soufflelang_souffle_Program_addType(JNIEnv *env, jobject obj1, jobject obj2) {
    LOG(INFO) ENTERJNI("addType");
    AstBuilder *prog = getHandle<AstBuilder>(env, obj1);
    AstType *ty = getHandle<AstType>(env, obj2);
    prog->addType(ty);
    LOG(INFO) LEAVEJNI;
}

void Java_com_soufflelang_souffle_Program_addClause(JNIEnv *env, jobject obj1, jobject obj2) {
    LOG(INFO) ENTERJNI("addClause");
    AstBuilder *prog = getHandle<AstBuilder>(env, obj1);
    AstClause *cl = getHandle<AstClause>(env, obj2);
    prog->addClause(cl);
    LOG(INFO) LEAVEJNI;
}

jstring Java_com_soufflelang_souffle_Program_print(JNIEnv *env, jobject obj1) {
    LOG(INFO) ENTERJNI("print");
    AstBuilder *prog = getHandle<AstBuilder>(env, obj1);
    std::string res = prog->print();
    LOG(INFO) LEAVEJNI;
    return env->NewStringUTF(res.c_str());
}

void Java_com_soufflelang_souffle_Program_compose(JNIEnv *env, jobject obj1, jobject obj2) {
    LOG(INFO) ENTERJNI("merge");
    AstBuilder *prog1 = getHandle<AstBuilder>(env, obj1);
    AstBuilder *prog2 = getHandle<AstBuilder>(env, obj2);
    prog1->compose(prog2);
    LOG(INFO) LEAVEJNI;
}

jobject Java_com_soufflelang_souffle_Program_makeSConstant(JNIEnv *env, jobject obj1, jstring str) {
    LOG(INFO) ENTERJNI("make SConstant");
    std::string cstr = std::string(env->GetStringUTFChars(str, 0));
    AstBuilder *prog = getHandle<AstBuilder>(env, obj1);
    AstStringConstant* sconst = prog->makeStringConstant(cstr);

    jclass c = env->FindClass("com/soufflelang/souffle/SConst");
    if (c == 0) {
      LOG(ERR) PRE << "Cannot Find class SConst" << "\n";
      assert(false);
    }

    jmethodID cnstrctr = env->GetMethodID(c, "<init>", "(J)V");
    if (cnstrctr == 0) {
      LOG(ERR) PRE << "Cannot find method SConst <init>" << "\n";
      assert(false);
    }

    LOG(INFO) LEAVEJNI;
    return env->NewObject(c, cnstrctr, sconst);
}
