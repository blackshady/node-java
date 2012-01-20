
#include "utils.h"
#include <string.h>
#include <algorithm>

std::list<jobject> javaReflectionGetMethods(JNIEnv *env, jclass clazz) {
  std::list<jobject> results;

  jclass clazzclazz = env->GetObjectClass(clazz);
  jmethodID methodId = env->GetMethodID(clazzclazz, "getMethods", "()[Ljava/lang/reflect/Method;");
  jobjectArray methodObjects = (jobjectArray)env->CallObjectMethod(clazz, methodId);
  jsize methodCount = env->GetArrayLength(methodObjects);
  for(jsize i=0; i<methodCount; i++) {
    jobject obj = env->GetObjectArrayElement(methodObjects, i);
    results.push_back(obj);
  }

  return results;
}

std::list<jobject> javaReflectionGetConstructors(JNIEnv *env, jclass clazz) {
  std::list<jobject> results;

  jclass clazzclazz = env->GetObjectClass(clazz);
  jmethodID methodId = env->GetMethodID(clazzclazz, "getConstructors", "()[Ljava/lang/reflect/Constructor;");
  jobjectArray methodObjects = (jobjectArray)env->CallObjectMethod(clazz, methodId);
  jsize methodCount = env->GetArrayLength(methodObjects);
  for(jsize i=0; i<methodCount; i++) {
    jobject obj = env->GetObjectArrayElement(methodObjects, i);
    results.push_back(obj);
  }

  return results;
}

std::string javaToString(JNIEnv *env, jstring str) {
  const char* chars = env->GetStringUTFChars(str, NULL);
  std::string results = chars;
  env->ReleaseStringUTFChars(str, chars);
  return results;
}

std::string javaObjectToString(JNIEnv *env, jobject obj) {
  if(obj == NULL) {
    return "";
  }
  jclass objClazz = env->GetObjectClass(obj);
  jmethodID methodId = env->GetMethodID(objClazz, "toString", "()Ljava/lang/String;");
  jstring result = (jstring)env->CallObjectMethod(obj, methodId);
  return javaToString(env, result);
}

jobject javaFindBestMatchingMethod(
  JNIEnv *env,
  std::list<jobject>& methods,
  const char *methodName,
  std::list<jobject>& args) {

  jclass methodClazz = env->FindClass("java/lang/reflect/Method");
  jmethodID method_getNameMethod = env->GetMethodID(methodClazz, "getName", "()Ljava/lang/String;");

  for(std::list<jobject>::iterator it = methods.begin(); it != methods.end(); it++) {
    std::string itMethodName = javaToString(env, (jstring)env->CallObjectMethod(*it, method_getNameMethod));
    if(itMethodName == methodName) {
      return *it; // TODO: check parameters
    }
  }
  return NULL;
}

jobject javaFindBestMatchingConstructor(
  JNIEnv *env,
  std::list<jobject>& constructors,
  std::list<jobject>& args) {

  jclass constructorClazz = env->FindClass("java/lang/reflect/Constructor");
  jmethodID constructor_getParameterTypes = env->GetMethodID(constructorClazz, "getParameterTypes", "()[Ljava/lang/Class;");

  for(std::list<jobject>::iterator it = constructors.begin(); it != constructors.end(); it++) {
    jarray parameters = (jarray)env->CallObjectMethod(*it, constructor_getParameterTypes);
    if(env->GetArrayLength(parameters) == (jsize)args.size()) {    
      return *it; // TODO: check parameters
    }
  }
  return NULL;
}

JNIEnv* javaAttachCurrentThread(JavaVM* jvm) {
  JNIEnv* env;
  JavaVMAttachArgs attachArgs;
  attachArgs.version = JNI_VERSION_1_4;
  attachArgs.name = NULL;
  attachArgs.group = NULL;
  jvm->AttachCurrentThread((void**)&env, &attachArgs);
  return env;
}

void javaDetachCurrentThread(JavaVM* jvm) {
  jvm->DetachCurrentThread();
}

jvalueType javaGetType(JNIEnv *env, jclass type) {
  // TODO: has to be a better way
  const char *typeStr = javaObjectToString(env, type).c_str();
  if(strcmp(typeStr, "int") == 0) {
    return TYPE_INT;
  } else if(strcmp(typeStr, "class java.lang.String") == 0) {
    return TYPE_STRING;
  }
  
  //jclass intClazz = env->FindClass("java/lang/Integer");

  return TYPE_OBJECT;  
}

jclass javaFindClass(JNIEnv* env, std::string className) {
  std::replace(className.begin(), className.end(), '.', '/');
  jclass clazz = env->FindClass(className.c_str());
  if(env->ExceptionCheck()) {
    env->ExceptionDescribe(); // TODO: handle error
    return NULL;
  }
  return clazz;
}

