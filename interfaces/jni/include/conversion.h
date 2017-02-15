#pragma once
#include <jni.h>
#include <vector>
#include <string>



inline std::vector<std::string> arr2vec(JNIEnv* env, jobject arrayList) {
  jclass java_util_ArrayList;
  jmethodID java_util_ArrayList_size;
  jmethodID java_util_ArrayList_get;

  java_util_ArrayList = 
    static_cast<jclass>(env->NewGlobalRef(env->FindClass("java/util/ArrayList")));
 
  java_util_ArrayList_size = env->GetMethodID(java_util_ArrayList, "size", "()I");
  java_util_ArrayList_get = env->GetMethodID(java_util_ArrayList, "get", "(I)Ljava/lang/Object;");

  jint len = env->CallIntMethod(arrayList, java_util_ArrayList_size);
  std::vector<std::string> result;
  result.reserve(len);
  for (jint i = 0; i < len; i++) {
    jstring element = 
      static_cast<jstring>(env->CallObjectMethod(arrayList, java_util_ArrayList_get, i));
    const char* pchars = env->GetStringUTFChars(element, nullptr);
    result.emplace_back(pchars);
    env->ReleaseStringUTFChars(element, pchars);
    env->DeleteLocalRef(element);
  }

  return result;
}

inline jobject vec2arr(JNIEnv* env, std::vector<std::string> vec) {

  jclass java_util_ArrayList;
  jmethodID java_util_ArrayList_;
  jmethodID java_util_ArrayList_add;

  java_util_ArrayList = 
    static_cast<jclass>(env->NewGlobalRef(env->FindClass("java/util/ArrayList")));
 
  java_util_ArrayList_ = env->GetMethodID(java_util_ArrayList,"<init>", "(I)V");
  java_util_ArrayList_add = env->GetMethodID(java_util_ArrayList, "add", "(Ljava/lang/Object;)Z");

  jobject result = env->NewObject(java_util_ArrayList, java_util_ArrayList_, vec.size());
  for (std::string s: vec) {
    jstring element = env->NewStringUTF(s.c_str());
    env->CallVoidMethod(result, java_util_ArrayList_add, element);
    env->DeleteLocalRef(element);
  }
  return result;
}
