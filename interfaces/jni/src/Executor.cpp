#include<jni.h>
#include "handle.h"

#include "com_soufflelang_souffle_Executor.h"

#include "Executor.h"
#include "InterfaceResult.h"

using namespace souffle;

void Java_com_soufflelang_souffle_Executor_release(JNIEnv* env, jobject obj1) {
    LOG(INFO) ENTERJNI("release");
    Executor* souffle = getHandle<Executor>(env, obj1);
    LOG(INFO) LEAVEJNI;
    delete souffle;
}

jobject Java_com_soufflelang_souffle_Executor_executeInterpreter(JNIEnv* env, jobject obj1, jobject input) {
    LOG(INFO) ENTERJNI("executeInterpreter");

    Executor* souffle = getHandle<Executor>(env, obj1);
    LOG(MEM) PRE << "Got executer " << souffle << "\n";
    assert(souffle != NULL && "Cannot get executor!!\n");

    RamData* data = getHandle<RamData>(env, input);
    LOG(MEM) PRE << "Got data " << data << "\n";
    assert(data != NULL && "Cannot get data!!\n");

    InterfaceResult* res = souffle->executeInterpreter(data);
    LOG(MEM) PRE << "Got interface result from interpreter execution " << res << "\n";
    assert(res != NULL && "Cannot get data!!\n");

    jclass c = env->FindClass("com/soufflelang/souffle/Result");
    if (c == 0) {
      LOG(ERR) PRE << "Find class Result failed.\n";
      assert(false && "Find class Result failed.\n");
    }

    jmethodID cnstrctr = env->GetMethodID(c, "<init>", "(J)V");
    if (cnstrctr == 0){
      LOG(ERR) PRE << "Find method <init> failed.\n";
      assert(false && "Find method <init> failed.\n");
    }

    LOG(INFO) LEAVEJNI;
    return env->NewObject(c, cnstrctr, res);
}

jobject Java_com_soufflelang_souffle_Executor_executeCompiler(JNIEnv* env, jobject obj1, jobject input, jstring jname) {
    LOG(INFO) ENTERJNI("executeCompiler");

    const char *nname = env->GetStringUTFChars(jname, 0);
    std::string name = std::string(nname);
    LOG(INFO) PRE << "Project name is "  << name <<"\n";
    assert(name != "" && "name is empty!!\n");

    Executor* souffle = getHandle<Executor>(env, obj1);
    LOG(MEM) PRE << "Got executor "  << souffle <<"\n";
    assert(souffle != NULL && "executor is NULL!!\n");

    RamData* data = getHandle<RamData>(env, input);
    LOG(MEM) PRE << "Got Ram data " << data <<"\n";
    assert(data != NULL && "data is NULL!!\n");

    InterfaceResult* res = souffle->executeCompiler(data, name);
    LOG(MEM) PRE << "Got interface result " << res <<"\n";
    assert(res != NULL && "result is NULL!!\n");

    jclass c = env->FindClass("com/soufflelang/souffle/Result");

    if (c == 0) {
      LOG(ERR) PRE << "Find class Result failed!\n";
      assert(false && "Find class Result failed!\n");
    }

    jmethodID cnstrctr = env->GetMethodID(c, "<init>", "(J)V");
    if (cnstrctr == 0){ 
      LOG(ERR) PRE << "Find method <init> failed!\n";
      assert(false && "Find method init failed!\n");
    }

    LOG(INFO) LEAVEJNI;
    return env->NewObject(c, cnstrctr, res);
}

void Java_com_soufflelang_souffle_Executor_compile(JNIEnv* env, jobject obj1, jstring jname) {
    const char *nname = env->GetStringUTFChars(jname, 0);
    std::string name = std::string(nname);
    LOG(INFO) PRE << "Project name is "  << name <<"\n";
    assert(name != "" && "name is empty!!\n"); 

    Executor* souffle = getHandle<Executor>(env, obj1);
    LOG(MEM) PRE << "Got executor "  << souffle <<"\n";
    assert(souffle != NULL && "executor is NULL!!\n");

    souffle->compile(name);

    LOG(INFO) LEAVEJNI;
}
