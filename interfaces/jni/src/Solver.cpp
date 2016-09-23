#include<jni.h>
#include "handle.h"

#include "com_soufflelang_souffle_Solver.h"

#include "Interfaces.h"
#include "InterfaceResult.h"

using namespace souffle;

void Java_com_soufflelang_souffle_Solver_init(JNIEnv *env, jobject obj) {
    Flags flags;
    flags.includeOpt="-I.";
    flags.factFileDir = "..";
    flags.outputDir="-";
    InternalInterface* souffle = new InternalInterface(flags);
    setHandle(env, obj, souffle);
}

void Java_com_soufflelang_souffle_Solver_release(JNIEnv *env, jobject obj) {
    LOG(INFO) ENTERJNI("release");
    InternalInterface* souffle = getHandle<InternalInterface>(env, obj);
    LOG(INFO) PRE << "Releasing internal interface " << souffle << "\n";
    LOG(INFO) LEAVEJNI;
    delete souffle;
}

jobject Java_com_soufflelang_souffle_Solver_parse(JNIEnv* env, jobject obj1, jobject obj2) {
    LOG(INFO) ENTERJNI("parse");
    InternalInterface* souffle = getHandle<InternalInterface>(env, obj1);
    LOG(INFO) PRE << "Got internal interface " << souffle << "\n";
    assert(souffle != NULL && "Cannot get internal interface!!");

    AstBuilder* builder = getHandle<AstBuilder>(env, obj2);
    LOG(INFO) PRE << "Got Ast Buidler " << builder << "\n";
    assert(builder != NULL && "Cannot get internal interface!!");

    LOG(INFO) PRE << "Parsing \n";
    Executor* ex = souffle->parse(builder);
    LOG(INFO) PRE << "Got executer " << ex << " form parse \n";
    assert(ex != NULL && "Executer is NULL!!!");

    // Return Executer
    jclass c = env->FindClass("com/soufflelang/souffle/Executor");
    if (c == 0) { 
      printf("Find class failed.\n"); 
      LOG(ERR) PRE << "Cant find class Executor!!!\n";
      assert( false && "Cant find class Executor!!");
    }

    jmethodID cnstrctr = env->GetMethodID(c, "<init>", "(J)V");
    if (cnstrctr == 0){ 
      LOG(ERR) PRE << "Cant find method  Executor <init>!!!\n";
      assert( false && "Cant find method Executor <init>!!");
    }

    LOG(INFO) LEAVEJNI;
    return env->NewObject(c, cnstrctr, ex);
}
