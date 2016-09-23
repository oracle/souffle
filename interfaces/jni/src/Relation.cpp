#include<jni.h>
#include "com_soufflelang_souffle_Relation.h"
#include "handle.h"
#include "AstRelation.h"

/* relation is read from csv file */ 
#define INPUT_RELATION (0x1)

/* relation is written to csv file */ 
#define OUTPUT_RELATION (0x2)

/* number of tuples are written to stdout */ 
#define PRINTSIZE_RELATION (0x4)

/* Rules of a relation defined in a component can be overwritten by sub-component */ 
#define OVERRIDABLE_RELATION (0x8)

#define DATA_RELATION (0x10)


using namespace souffle;

void Java_com_soufflelang_souffle_Relation_init(JNIEnv* env, jobject obj, jstring str) {
  const char *cStr = env->GetStringUTFChars(str, NULL);
  if (NULL == cStr) return;

  AstRelation* rel = new AstRelation();
  rel->setName(std::string(cStr));
  setHandle(env, obj, rel);
}

void Java_com_soufflelang_souffle_Relation_addAttribute (JNIEnv* env, jobject obj, jstring str1, jstring str2) {
  const char *cStr1 = env->GetStringUTFChars(str1, NULL);
  if (NULL == cStr1) return;

  const char *cStr2 = env->GetStringUTFChars(str2, NULL);
  if (NULL == cStr2) return;

  AstRelation *r = getHandle<AstRelation>(env, obj);
  r->addAttribute(std::unique_ptr<AstAttribute>(new AstAttribute(std::string(cStr1), AstTypeIdentifier(std::string(cStr2)))));
}

void Java_com_soufflelang_souffle_Relation_setAsInputData(JNIEnv* env, jobject obj) {
  AstRelation *r = getHandle<AstRelation>(env, obj);
  r->setQualifier(INPUT_RELATION | DATA_RELATION);
}

void Java_com_soufflelang_souffle_Relation_setAsOutputData(JNIEnv* env, jobject obj) {
  AstRelation *r = getHandle<AstRelation>(env, obj);
  r->setQualifier(OUTPUT_RELATION | DATA_RELATION);
}

void Java_com_soufflelang_souffle_Relation_setAsInput(JNIEnv* env, jobject obj) {
  AstRelation *r = getHandle<AstRelation>(env, obj);
  r->setQualifier(INPUT_RELATION | DATA_RELATION);
}

void Java_com_soufflelang_souffle_Relation_setAsOutput(JNIEnv* env, jobject obj) {
  AstRelation *r = getHandle<AstRelation>(env, obj);
  r->setQualifier(OUTPUT_RELATION);
}


