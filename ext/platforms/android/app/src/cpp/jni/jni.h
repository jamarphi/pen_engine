#include "../../../../../../../src/state/config.h"

#ifdef __PEN_MOBILE__
#ifndef __PEN_LINUX__ /*For Windows*/

#ifndef LIBNATIVEHELPER_INCLUDE_JNI_JNI_H_
#define LIBNATIVEHELPER_INCLUDE_JNI_JNI_H_

#include <stdio.h>
#include <stdarg.h>

/* jni_md.h contains the machine-dependent typedefs for jbyte, jint
   and jlong */

#include "jni_md.h"

#ifdef __cplusplus
extern "C" {
#endif

    /*
     * JNI Types
     */

#ifndef JNI_TYPES_ALREADY_DEFINED_IN_JNI_MD_H

    typedef unsigned char   jboolean;
    typedef unsigned short  jchar;
    typedef short           jshort;
    typedef float           jfloat;
    typedef double          jdouble;

    typedef jint            jsize;

#ifdef __cplusplus

    class _jobject {};
    class _jclass : public _jobject {};
    class _jthrowable : public _jobject {};
    class _jstring : public _jobject {};
    class _jarray : public _jobject {};
    class _jbooleanArray : public _jarray {};
    class _jbyteArray : public _jarray {};
    class _jcharArray : public _jarray {};
    class _jshortArray : public _jarray {};
    class _jintArray : public _jarray {};
    class _jlongArray : public _jarray {};
    class _jfloatArray : public _jarray {};
    class _jdoubleArray : public _jarray {};
    class _jobjectArray : public _jarray {};

    typedef _jobject* jobject;
    typedef _jclass* jclass;
    typedef _jthrowable* jthrowable;
    typedef _jstring* jstring;
    typedef _jarray* jarray;
    typedef _jbooleanArray* jbooleanArray;
    typedef _jbyteArray* jbyteArray;
    typedef _jcharArray* jcharArray;
    typedef _jshortArray* jshortArray;
    typedef _jintArray* jintArray;
    typedef _jlongArray* jlongArray;
    typedef _jfloatArray* jfloatArray;
    typedef _jdoubleArray* jdoubleArray;
    typedef _jobjectArray* jobjectArray;

#else

    struct _jobject;

    typedef struct _jobject* jobject;
    typedef jobject jclass;
    typedef jobject jthrowable;
    typedef jobject jstring;
    typedef jobject jarray;
    typedef jarray jbooleanArray;
    typedef jarray jbyteArray;
    typedef jarray jcharArray;
    typedef jarray jshortArray;
    typedef jarray jintArray;
    typedef jarray jlongArray;
    typedef jarray jfloatArray;
    typedef jarray jdoubleArray;
    typedef jarray jobjectArray;

#endif

    typedef jobject jweak;

    typedef union jvalue {
        jboolean z;
        jbyte    b;
        jchar    c;
        jshort   s;
        jint     i;
        jlong    j;
        jfloat   f;
        jdouble  d;
        jobject  l;
    } jvalue;

    struct _jfieldID;
    typedef struct _jfieldID* jfieldID;

    struct _jmethodID;
    typedef struct _jmethodID* jmethodID;

    /* Return values from jobjectRefType */
    typedef enum _jobjectType {
        JNIInvalidRefType = 0,
        JNILocalRefType = 1,
        JNIGlobalRefType = 2,
        JNIWeakGlobalRefType = 3
    } jobjectRefType;


#endif /* JNI_TYPES_ALREADY_DEFINED_IN_JNI_MD_H */

    /*
     * jboolean constants
     */

#define JNI_FALSE 0
#define JNI_TRUE 1

     /*
      * possible return values for JNI functions.
      */

#define JNI_OK           0                 /* success */
#define JNI_ERR          (-1)              /* unknown error */
#define JNI_EDETACHED    (-2)              /* thread detached from the VM */
#define JNI_EVERSION     (-3)              /* JNI version error */
#define JNI_ENOMEM       (-4)              /* not enough memory */
#define JNI_EEXIST       (-5)              /* VM already created */
#define JNI_EINVAL       (-6)              /* invalid arguments */

      /*
       * used in ReleaseScalarArrayElements
       */

#define JNI_COMMIT 1
#define JNI_ABORT 2

       /*
        * used in RegisterNatives to describe native method name, signature,
        * and function pointer.
        */

    typedef struct {
        char* name;
        char* signature;
        void* fnPtr;
    } JNINativeMethod;

    /*
     * JNI Native Method Interface.
     */

    struct JNINativeInterface_;

    struct JNIEnv_;

#ifdef __cplusplus
    typedef JNIEnv_ JNIEnv;
#else
    typedef const struct JNINativeInterface_* JNIEnv;
#endif

    /*
     * JNI Invocation Interface.
     */

    struct JNIInvokeInterface_;

    struct JavaVM_;

#ifdef __cplusplus
    typedef JavaVM_ JavaVM;
#else
    typedef const struct JNIInvokeInterface_* JavaVM;
#endif

    struct JNINativeInterface_ {
        void* reserved0;
        void* reserved1;
        void* reserved2;

        void* reserved3;
        jint(JNICALL* GetVersion)(JNIEnv* env);

        jclass(JNICALL* DefineClass)
            (JNIEnv* env, const char* name, jobject loader, const jbyte* buf,
                jsize len);
        jclass(JNICALL* FindClass)
            (JNIEnv* env, const char* name);

        jmethodID(JNICALL* FromReflectedMethod)
            (JNIEnv* env, jobject method);
        jfieldID(JNICALL* FromReflectedField)
            (JNIEnv* env, jobject field);

        jobject(JNICALL* ToReflectedMethod)
            (JNIEnv* env, jclass cls, jmethodID methodID, jboolean isStatic);

        jclass(JNICALL* GetSuperclass)
            (JNIEnv* env, jclass sub);
        jboolean(JNICALL* IsAssignableFrom)
            (JNIEnv* env, jclass sub, jclass sup);

        jobject(JNICALL* ToReflectedField)
            (JNIEnv* env, jclass cls, jfieldID fieldID, jboolean isStatic);

        jint(JNICALL* Throw)
            (JNIEnv* env, jthrowable obj);
        jint(JNICALL* ThrowNew)
            (JNIEnv* env, jclass clazz, const char* msg);
        jthrowable(JNICALL* ExceptionOccurred)
            (JNIEnv* env);
        void (JNICALL* ExceptionDescribe)
            (JNIEnv* env);
        void (JNICALL* ExceptionClear)
            (JNIEnv* env);
        void (JNICALL* FatalError)
            (JNIEnv* env, const char* msg);

        jint(JNICALL* PushLocalFrame)
            (JNIEnv* env, jint capacity);
        jobject(JNICALL* PopLocalFrame)
            (JNIEnv* env, jobject result);

        jobject(JNICALL* NewGlobalRef)
            (JNIEnv* env, jobject lobj);
        void (JNICALL* DeleteGlobalRef)
            (JNIEnv* env, jobject gref);
        void (JNICALL* DeleteLocalRef)
            (JNIEnv* env, jobject obj);
        jboolean(JNICALL* IsSameObject)
            (JNIEnv* env, jobject obj1, jobject obj2);
        jobject(JNICALL* NewLocalRef)
            (JNIEnv* env, jobject ref);
        jint(JNICALL* EnsureLocalCapacity)
            (JNIEnv* env, jint capacity);

        jobject(JNICALL* AllocObject)
            (JNIEnv* env, jclass clazz);
        jobject(JNICALL* NewObject)
            (JNIEnv* env, jclass clazz, jmethodID methodID, ...);
        jobject(JNICALL* NewObjectV)
            (JNIEnv* env, jclass clazz, jmethodID methodID, va_list args);
        jobject(JNICALL* NewObjectA)
            (JNIEnv* env, jclass clazz, jmethodID methodID, const jvalue* args);

        jclass(JNICALL* GetObjectClass)
            (JNIEnv* env, jobject obj);
        jboolean(JNICALL* IsInstanceOf)
            (JNIEnv* env, jobject obj, jclass clazz);

        jmethodID(JNICALL* GetMethodID)
            (JNIEnv* env, jclass clazz, const char* name, const char* sig);

        jobject(JNICALL* CallObjectMethod)
            (JNIEnv* env, jobject obj, jmethodID methodID, ...);
        jobject(JNICALL* CallObjectMethodV)
            (JNIEnv* env, jobject obj, jmethodID methodID, va_list args);
        jobject(JNICALL* CallObjectMethodA)
            (JNIEnv* env, jobject obj, jmethodID methodID, const jvalue* args);

        jboolean(JNICALL* CallBooleanMethod)
            (JNIEnv* env, jobject obj, jmethodID methodID, ...);
        jboolean(JNICALL* CallBooleanMethodV)
            (JNIEnv* env, jobject obj, jmethodID methodID, va_list args);
        jboolean(JNICALL* CallBooleanMethodA)
            (JNIEnv* env, jobject obj, jmethodID methodID, const jvalue* args);

        jbyte(JNICALL* CallByteMethod)
            (JNIEnv* env, jobject obj, jmethodID methodID, ...);
        jbyte(JNICALL* CallByteMethodV)
            (JNIEnv* env, jobject obj, jmethodID methodID, va_list args);
        jbyte(JNICALL* CallByteMethodA)
            (JNIEnv* env, jobject obj, jmethodID methodID, const jvalue* args);

        jchar(JNICALL* CallCharMethod)
            (JNIEnv* env, jobject obj, jmethodID methodID, ...);
        jchar(JNICALL* CallCharMethodV)
            (JNIEnv* env, jobject obj, jmethodID methodID, va_list args);
        jchar(JNICALL* CallCharMethodA)
            (JNIEnv* env, jobject obj, jmethodID methodID, const jvalue* args);

        jshort(JNICALL* CallShortMethod)
            (JNIEnv* env, jobject obj, jmethodID methodID, ...);
        jshort(JNICALL* CallShortMethodV)
            (JNIEnv* env, jobject obj, jmethodID methodID, va_list args);
        jshort(JNICALL* CallShortMethodA)
            (JNIEnv* env, jobject obj, jmethodID methodID, const jvalue* args);

        jint(JNICALL* CallIntMethod)
            (JNIEnv* env, jobject obj, jmethodID methodID, ...);
        jint(JNICALL* CallIntMethodV)
            (JNIEnv* env, jobject obj, jmethodID methodID, va_list args);
        jint(JNICALL* CallIntMethodA)
            (JNIEnv* env, jobject obj, jmethodID methodID, const jvalue* args);

        jlong(JNICALL* CallLongMethod)
            (JNIEnv* env, jobject obj, jmethodID methodID, ...);
        jlong(JNICALL* CallLongMethodV)
            (JNIEnv* env, jobject obj, jmethodID methodID, va_list args);
        jlong(JNICALL* CallLongMethodA)
            (JNIEnv* env, jobject obj, jmethodID methodID, const jvalue* args);

        jfloat(JNICALL* CallFloatMethod)
            (JNIEnv* env, jobject obj, jmethodID methodID, ...);
        jfloat(JNICALL* CallFloatMethodV)
            (JNIEnv* env, jobject obj, jmethodID methodID, va_list args);
        jfloat(JNICALL* CallFloatMethodA)
            (JNIEnv* env, jobject obj, jmethodID methodID, const jvalue* args);

        jdouble(JNICALL* CallDoubleMethod)
            (JNIEnv* env, jobject obj, jmethodID methodID, ...);
        jdouble(JNICALL* CallDoubleMethodV)
            (JNIEnv* env, jobject obj, jmethodID methodID, va_list args);
        jdouble(JNICALL* CallDoubleMethodA)
            (JNIEnv* env, jobject obj, jmethodID methodID, const jvalue* args);

        void (JNICALL* CallVoidMethod)
            (JNIEnv* env, jobject obj, jmethodID methodID, ...);
        void (JNICALL* CallVoidMethodV)
            (JNIEnv* env, jobject obj, jmethodID methodID, va_list args);
        void (JNICALL* CallVoidMethodA)
            (JNIEnv* env, jobject obj, jmethodID methodID, const jvalue* args);

        jobject(JNICALL* CallNonvirtualObjectMethod)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID, ...);
        jobject(JNICALL* CallNonvirtualObjectMethodV)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID,
                va_list args);
        jobject(JNICALL* CallNonvirtualObjectMethodA)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID,
                const jvalue* args);

        jboolean(JNICALL* CallNonvirtualBooleanMethod)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID, ...);
        jboolean(JNICALL* CallNonvirtualBooleanMethodV)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID,
                va_list args);
        jboolean(JNICALL* CallNonvirtualBooleanMethodA)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID,
                const jvalue* args);

        jbyte(JNICALL* CallNonvirtualByteMethod)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID, ...);
        jbyte(JNICALL* CallNonvirtualByteMethodV)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID,
                va_list args);
        jbyte(JNICALL* CallNonvirtualByteMethodA)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID,
                const jvalue* args);

        jchar(JNICALL* CallNonvirtualCharMethod)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID, ...);
        jchar(JNICALL* CallNonvirtualCharMethodV)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID,
                va_list args);
        jchar(JNICALL* CallNonvirtualCharMethodA)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID,
                const jvalue* args);

        jshort(JNICALL* CallNonvirtualShortMethod)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID, ...);
        jshort(JNICALL* CallNonvirtualShortMethodV)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID,
                va_list args);
        jshort(JNICALL* CallNonvirtualShortMethodA)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID,
                const jvalue* args);

        jint(JNICALL* CallNonvirtualIntMethod)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID, ...);
        jint(JNICALL* CallNonvirtualIntMethodV)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID,
                va_list args);
        jint(JNICALL* CallNonvirtualIntMethodA)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID,
                const jvalue* args);

        jlong(JNICALL* CallNonvirtualLongMethod)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID, ...);
        jlong(JNICALL* CallNonvirtualLongMethodV)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID,
                va_list args);
        jlong(JNICALL* CallNonvirtualLongMethodA)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID,
                const jvalue* args);

        jfloat(JNICALL* CallNonvirtualFloatMethod)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID, ...);
        jfloat(JNICALL* CallNonvirtualFloatMethodV)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID,
                va_list args);
        jfloat(JNICALL* CallNonvirtualFloatMethodA)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID,
                const jvalue* args);

        jdouble(JNICALL* CallNonvirtualDoubleMethod)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID, ...);
        jdouble(JNICALL* CallNonvirtualDoubleMethodV)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID,
                va_list args);
        jdouble(JNICALL* CallNonvirtualDoubleMethodA)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID,
                const jvalue* args);

        void (JNICALL* CallNonvirtualVoidMethod)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID, ...);
        void (JNICALL* CallNonvirtualVoidMethodV)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID,
                va_list args);
        void (JNICALL* CallNonvirtualVoidMethodA)
            (JNIEnv* env, jobject obj, jclass clazz, jmethodID methodID,
                const jvalue* args);

        jfieldID(JNICALL* GetFieldID)
            (JNIEnv* env, jclass clazz, const char* name, const char* sig);

        jobject(JNICALL* GetObjectField)
            (JNIEnv* env, jobject obj, jfieldID fieldID);
        jboolean(JNICALL* GetBooleanField)
            (JNIEnv* env, jobject obj, jfieldID fieldID);
        jbyte(JNICALL* GetByteField)
            (JNIEnv* env, jobject obj, jfieldID fieldID);
        jchar(JNICALL* GetCharField)
            (JNIEnv* env, jobject obj, jfieldID fieldID);
        jshort(JNICALL* GetShortField)
            (JNIEnv* env, jobject obj, jfieldID fieldID);
        jint(JNICALL* GetIntField)
            (JNIEnv* env, jobject obj, jfieldID fieldID);
        jlong(JNICALL* GetLongField)
            (JNIEnv* env, jobject obj, jfieldID fieldID);
        jfloat(JNICALL* GetFloatField)
            (JNIEnv* env, jobject obj, jfieldID fieldID);
        jdouble(JNICALL* GetDoubleField)
            (JNIEnv* env, jobject obj, jfieldID fieldID);

        void (JNICALL* SetObjectField)
            (JNIEnv* env, jobject obj, jfieldID fieldID, jobject val);
        void (JNICALL* SetBooleanField)
            (JNIEnv* env, jobject obj, jfieldID fieldID, jboolean val);
        void (JNICALL* SetByteField)
            (JNIEnv* env, jobject obj, jfieldID fieldID, jbyte val);
        void (JNICALL* SetCharField)
            (JNIEnv* env, jobject obj, jfieldID fieldID, jchar val);
        void (JNICALL* SetShortField)
            (JNIEnv* env, jobject obj, jfieldID fieldID, jshort val);
        void (JNICALL* SetIntField)
            (JNIEnv* env, jobject obj, jfieldID fieldID, jint val);
        void (JNICALL* SetLongField)
            (JNIEnv* env, jobject obj, jfieldID fieldID, jlong val);
        void (JNICALL* SetFloatField)
            (JNIEnv* env, jobject obj, jfieldID fieldID, jfloat val);
        void (JNICALL* SetDoubleField)
            (JNIEnv* env, jobject obj, jfieldID fieldID, jdouble val);

        jmethodID(JNICALL* GetStaticMethodID)
            (JNIEnv* env, jclass clazz, const char* name, const char* sig);

        jobject(JNICALL* CallStaticObjectMethod)
            (JNIEnv* env, jclass clazz, jmethodID methodID, ...);
        jobject(JNICALL* CallStaticObjectMethodV)
            (JNIEnv* env, jclass clazz, jmethodID methodID, va_list args);
        jobject(JNICALL* CallStaticObjectMethodA)
            (JNIEnv* env, jclass clazz, jmethodID methodID, const jvalue* args);

        jboolean(JNICALL* CallStaticBooleanMethod)
            (JNIEnv* env, jclass clazz, jmethodID methodID, ...);
        jboolean(JNICALL* CallStaticBooleanMethodV)
            (JNIEnv* env, jclass clazz, jmethodID methodID, va_list args);
        jboolean(JNICALL* CallStaticBooleanMethodA)
            (JNIEnv* env, jclass clazz, jmethodID methodID, const jvalue* args);

        jbyte(JNICALL* CallStaticByteMethod)
            (JNIEnv* env, jclass clazz, jmethodID methodID, ...);
        jbyte(JNICALL* CallStaticByteMethodV)
            (JNIEnv* env, jclass clazz, jmethodID methodID, va_list args);
        jbyte(JNICALL* CallStaticByteMethodA)
            (JNIEnv* env, jclass clazz, jmethodID methodID, const jvalue* args);

        jchar(JNICALL* CallStaticCharMethod)
            (JNIEnv* env, jclass clazz, jmethodID methodID, ...);
        jchar(JNICALL* CallStaticCharMethodV)
            (JNIEnv* env, jclass clazz, jmethodID methodID, va_list args);
        jchar(JNICALL* CallStaticCharMethodA)
            (JNIEnv* env, jclass clazz, jmethodID methodID, const jvalue* args);

        jshort(JNICALL* CallStaticShortMethod)
            (JNIEnv* env, jclass clazz, jmethodID methodID, ...);
        jshort(JNICALL* CallStaticShortMethodV)
            (JNIEnv* env, jclass clazz, jmethodID methodID, va_list args);
        jshort(JNICALL* CallStaticShortMethodA)
            (JNIEnv* env, jclass clazz, jmethodID methodID, const jvalue* args);

        jint(JNICALL* CallStaticIntMethod)
            (JNIEnv* env, jclass clazz, jmethodID methodID, ...);
        jint(JNICALL* CallStaticIntMethodV)
            (JNIEnv* env, jclass clazz, jmethodID methodID, va_list args);
        jint(JNICALL* CallStaticIntMethodA)
            (JNIEnv* env, jclass clazz, jmethodID methodID, const jvalue* args);

        jlong(JNICALL* CallStaticLongMethod)
            (JNIEnv* env, jclass clazz, jmethodID methodID, ...);
        jlong(JNICALL* CallStaticLongMethodV)
            (JNIEnv* env, jclass clazz, jmethodID methodID, va_list args);
        jlong(JNICALL* CallStaticLongMethodA)
            (JNIEnv* env, jclass clazz, jmethodID methodID, const jvalue* args);

        jfloat(JNICALL* CallStaticFloatMethod)
            (JNIEnv* env, jclass clazz, jmethodID methodID, ...);
        jfloat(JNICALL* CallStaticFloatMethodV)
            (JNIEnv* env, jclass clazz, jmethodID methodID, va_list args);
        jfloat(JNICALL* CallStaticFloatMethodA)
            (JNIEnv* env, jclass clazz, jmethodID methodID, const jvalue* args);

        jdouble(JNICALL* CallStaticDoubleMethod)
            (JNIEnv* env, jclass clazz, jmethodID methodID, ...);
        jdouble(JNICALL* CallStaticDoubleMethodV)
            (JNIEnv* env, jclass clazz, jmethodID methodID, va_list args);
        jdouble(JNICALL* CallStaticDoubleMethodA)
            (JNIEnv* env, jclass clazz, jmethodID methodID, const jvalue* args);

        void (JNICALL* CallStaticVoidMethod)
            (JNIEnv* env, jclass cls, jmethodID methodID, ...);
        void (JNICALL* CallStaticVoidMethodV)
            (JNIEnv* env, jclass cls, jmethodID methodID, va_list args);
        void (JNICALL* CallStaticVoidMethodA)
            (JNIEnv* env, jclass cls, jmethodID methodID, const jvalue* args);

        jfieldID(JNICALL* GetStaticFieldID)
            (JNIEnv* env, jclass clazz, const char* name, const char* sig);
        jobject(JNICALL* GetStaticObjectField)
            (JNIEnv* env, jclass clazz, jfieldID fieldID);
        jboolean(JNICALL* GetStaticBooleanField)
            (JNIEnv* env, jclass clazz, jfieldID fieldID);
        jbyte(JNICALL* GetStaticByteField)
            (JNIEnv* env, jclass clazz, jfieldID fieldID);
        jchar(JNICALL* GetStaticCharField)
            (JNIEnv* env, jclass clazz, jfieldID fieldID);
        jshort(JNICALL* GetStaticShortField)
            (JNIEnv* env, jclass clazz, jfieldID fieldID);
        jint(JNICALL* GetStaticIntField)
            (JNIEnv* env, jclass clazz, jfieldID fieldID);
        jlong(JNICALL* GetStaticLongField)
            (JNIEnv* env, jclass clazz, jfieldID fieldID);
        jfloat(JNICALL* GetStaticFloatField)
            (JNIEnv* env, jclass clazz, jfieldID fieldID);
        jdouble(JNICALL* GetStaticDoubleField)
            (JNIEnv* env, jclass clazz, jfieldID fieldID);

        void (JNICALL* SetStaticObjectField)
            (JNIEnv* env, jclass clazz, jfieldID fieldID, jobject value);
        void (JNICALL* SetStaticBooleanField)
            (JNIEnv* env, jclass clazz, jfieldID fieldID, jboolean value);
        void (JNICALL* SetStaticByteField)
            (JNIEnv* env, jclass clazz, jfieldID fieldID, jbyte value);
        void (JNICALL* SetStaticCharField)
            (JNIEnv* env, jclass clazz, jfieldID fieldID, jchar value);
        void (JNICALL* SetStaticShortField)
            (JNIEnv* env, jclass clazz, jfieldID fieldID, jshort value);
        void (JNICALL* SetStaticIntField)
            (JNIEnv* env, jclass clazz, jfieldID fieldID, jint value);
        void (JNICALL* SetStaticLongField)
            (JNIEnv* env, jclass clazz, jfieldID fieldID, jlong value);
        void (JNICALL* SetStaticFloatField)
            (JNIEnv* env, jclass clazz, jfieldID fieldID, jfloat value);
        void (JNICALL* SetStaticDoubleField)
            (JNIEnv* env, jclass clazz, jfieldID fieldID, jdouble value);

        jstring(JNICALL* NewString)
            (JNIEnv* env, const jchar* unicode, jsize len);
        jsize(JNICALL* GetStringLength)
            (JNIEnv* env, jstring str);
        const jchar* (JNICALL* GetStringChars)
            (JNIEnv* env, jstring str, jboolean* isCopy);
        void (JNICALL* ReleaseStringChars)
            (JNIEnv* env, jstring str, const jchar* chars);

        jstring(JNICALL* NewStringUTF)
            (JNIEnv* env, const char* utf);
        jsize(JNICALL* GetStringUTFLength)
            (JNIEnv* env, jstring str);
        const char* (JNICALL* GetStringUTFChars)
            (JNIEnv* env, jstring str, jboolean* isCopy);
        void (JNICALL* ReleaseStringUTFChars)
            (JNIEnv* env, jstring str, const char* chars);


        jsize(JNICALL* GetArrayLength)
            (JNIEnv* env, jarray array);

        jobjectArray(JNICALL* NewObjectArray)
            (JNIEnv* env, jsize len, jclass clazz, jobject init);
        jobject(JNICALL* GetObjectArrayElement)
            (JNIEnv* env, jobjectArray array, jsize index);
        void (JNICALL* SetObjectArrayElement)
            (JNIEnv* env, jobjectArray array, jsize index, jobject val);

        jbooleanArray(JNICALL* NewBooleanArray)
            (JNIEnv* env, jsize len);
        jbyteArray(JNICALL* NewByteArray)
            (JNIEnv* env, jsize len);
        jcharArray(JNICALL* NewCharArray)
            (JNIEnv* env, jsize len);
        jshortArray(JNICALL* NewShortArray)
            (JNIEnv* env, jsize len);
        jintArray(JNICALL* NewIntArray)
            (JNIEnv* env, jsize len);
        jlongArray(JNICALL* NewLongArray)
            (JNIEnv* env, jsize len);
        jfloatArray(JNICALL* NewFloatArray)
            (JNIEnv* env, jsize len);
        jdoubleArray(JNICALL* NewDoubleArray)
            (JNIEnv* env, jsize len);

        jboolean* (JNICALL* GetBooleanArrayElements)
            (JNIEnv* env, jbooleanArray array, jboolean* isCopy);
        jbyte* (JNICALL* GetByteArrayElements)
            (JNIEnv* env, jbyteArray array, jboolean* isCopy);
        jchar* (JNICALL* GetCharArrayElements)
            (JNIEnv* env, jcharArray array, jboolean* isCopy);
        jshort* (JNICALL* GetShortArrayElements)
            (JNIEnv* env, jshortArray array, jboolean* isCopy);
        jint* (JNICALL* GetIntArrayElements)
            (JNIEnv* env, jintArray array, jboolean* isCopy);
        jlong* (JNICALL* GetLongArrayElements)
            (JNIEnv* env, jlongArray array, jboolean* isCopy);
        jfloat* (JNICALL* GetFloatArrayElements)
            (JNIEnv* env, jfloatArray array, jboolean* isCopy);
        jdouble* (JNICALL* GetDoubleArrayElements)
            (JNIEnv* env, jdoubleArray array, jboolean* isCopy);

        void (JNICALL* ReleaseBooleanArrayElements)
            (JNIEnv* env, jbooleanArray array, jboolean* elems, jint mode);
        void (JNICALL* ReleaseByteArrayElements)
            (JNIEnv* env, jbyteArray array, jbyte* elems, jint mode);
        void (JNICALL* ReleaseCharArrayElements)
            (JNIEnv* env, jcharArray array, jchar* elems, jint mode);
        void (JNICALL* ReleaseShortArrayElements)
            (JNIEnv* env, jshortArray array, jshort* elems, jint mode);
        void (JNICALL* ReleaseIntArrayElements)
            (JNIEnv* env, jintArray array, jint* elems, jint mode);
        void (JNICALL* ReleaseLongArrayElements)
            (JNIEnv* env, jlongArray array, jlong* elems, jint mode);
        void (JNICALL* ReleaseFloatArrayElements)
            (JNIEnv* env, jfloatArray array, jfloat* elems, jint mode);
        void (JNICALL* ReleaseDoubleArrayElements)
            (JNIEnv* env, jdoubleArray array, jdouble* elems, jint mode);

        void (JNICALL* GetBooleanArrayRegion)
            (JNIEnv* env, jbooleanArray array, jsize start, jsize l, jboolean* buf);
        void (JNICALL* GetByteArrayRegion)
            (JNIEnv* env, jbyteArray array, jsize start, jsize len, jbyte* buf);
        void (JNICALL* GetCharArrayRegion)
            (JNIEnv* env, jcharArray array, jsize start, jsize len, jchar* buf);
        void (JNICALL* GetShortArrayRegion)
            (JNIEnv* env, jshortArray array, jsize start, jsize len, jshort* buf);
        void (JNICALL* GetIntArrayRegion)
            (JNIEnv* env, jintArray array, jsize start, jsize len, jint* buf);
        void (JNICALL* GetLongArrayRegion)
            (JNIEnv* env, jlongArray array, jsize start, jsize len, jlong* buf);
        void (JNICALL* GetFloatArrayRegion)
            (JNIEnv* env, jfloatArray array, jsize start, jsize len, jfloat* buf);
        void (JNICALL* GetDoubleArrayRegion)
            (JNIEnv* env, jdoubleArray array, jsize start, jsize len, jdouble* buf);

        void (JNICALL* SetBooleanArrayRegion)
            (JNIEnv* env, jbooleanArray array, jsize start, jsize l, const jboolean* buf);
        void (JNICALL* SetByteArrayRegion)
            (JNIEnv* env, jbyteArray array, jsize start, jsize len, const jbyte* buf);
        void (JNICALL* SetCharArrayRegion)
            (JNIEnv* env, jcharArray array, jsize start, jsize len, const jchar* buf);
        void (JNICALL* SetShortArrayRegion)
            (JNIEnv* env, jshortArray array, jsize start, jsize len, const jshort* buf);
        void (JNICALL* SetIntArrayRegion)
            (JNIEnv* env, jintArray array, jsize start, jsize len, const jint* buf);
        void (JNICALL* SetLongArrayRegion)
            (JNIEnv* env, jlongArray array, jsize start, jsize len, const jlong* buf);
        void (JNICALL* SetFloatArrayRegion)
            (JNIEnv* env, jfloatArray array, jsize start, jsize len, const jfloat* buf);
        void (JNICALL* SetDoubleArrayRegion)
            (JNIEnv* env, jdoubleArray array, jsize start, jsize len, const jdouble* buf);

        jint(JNICALL* RegisterNatives)
            (JNIEnv* env, jclass clazz, const JNINativeMethod* methods,
                jint nMethods);
        jint(JNICALL* UnregisterNatives)
            (JNIEnv* env, jclass clazz);

        jint(JNICALL* MonitorEnter)
            (JNIEnv* env, jobject obj);
        jint(JNICALL* MonitorExit)
            (JNIEnv* env, jobject obj);

        jint(JNICALL* GetJavaVM)
            (JNIEnv* env, JavaVM** vm);

        void (JNICALL* GetStringRegion)
            (JNIEnv* env, jstring str, jsize start, jsize len, jchar* buf);
        void (JNICALL* GetStringUTFRegion)
            (JNIEnv* env, jstring str, jsize start, jsize len, char* buf);

        void* (JNICALL* GetPrimitiveArrayCritical)
            (JNIEnv* env, jarray array, jboolean* isCopy);
        void (JNICALL* ReleasePrimitiveArrayCritical)
            (JNIEnv* env, jarray array, void* carray, jint mode);

        const jchar* (JNICALL* GetStringCritical)
            (JNIEnv* env, jstring string, jboolean* isCopy);
        void (JNICALL* ReleaseStringCritical)
            (JNIEnv* env, jstring string, const jchar* cstring);

        jweak(JNICALL* NewWeakGlobalRef)
            (JNIEnv* env, jobject obj);
        void (JNICALL* DeleteWeakGlobalRef)
            (JNIEnv* env, jweak ref);

        jboolean(JNICALL* ExceptionCheck)
            (JNIEnv* env);

        jobject(JNICALL* NewDirectByteBuffer)
            (JNIEnv* env, void* address, jlong capacity);
        void* (JNICALL* GetDirectBufferAddress)
            (JNIEnv* env, jobject buf);
        jlong(JNICALL* GetDirectBufferCapacity)
            (JNIEnv* env, jobject buf);

        /* New JNI 1.6 Features */

        jobjectRefType(JNICALL* GetObjectRefType)
            (JNIEnv* env, jobject obj);

        /* Module Features */

        jobject(JNICALL* GetModule)
            (JNIEnv* env, jclass clazz);
    };

    /*
     * We use inlined functions for C++ so that programmers can write:
     *
     *    env->FindClass("java/lang/String")
     *
     * in C++ rather than:
     *
     *    (*env)->FindClass(env, "java/lang/String")
     *
     * in C.
     */

    struct JNIEnv_ {
        const struct JNINativeInterface_* functions;
#ifdef __cplusplus

        jint GetVersion() {
            return functions->GetVersion(this);
        }
        jclass DefineClass(const char* name, jobject loader, const jbyte* buf,
            jsize len) {
            return functions->DefineClass(this, name, loader, buf, len);
        }
        jclass FindClass(const char* name) {
            return functions->FindClass(this, name);
        }
        jmethodID FromReflectedMethod(jobject method) {
            return functions->FromReflectedMethod(this, method);
        }
        jfieldID FromReflectedField(jobject field) {
            return functions->FromReflectedField(this, field);
        }

        jobject ToReflectedMethod(jclass cls, jmethodID methodID, jboolean isStatic) {
            return functions->ToReflectedMethod(this, cls, methodID, isStatic);
        }

        jclass GetSuperclass(jclass sub) {
            return functions->GetSuperclass(this, sub);
        }
        jboolean IsAssignableFrom(jclass sub, jclass sup) {
            return functions->IsAssignableFrom(this, sub, sup);
        }

        jobject ToReflectedField(jclass cls, jfieldID fieldID, jboolean isStatic) {
            return functions->ToReflectedField(this, cls, fieldID, isStatic);
        }

        jint Throw(jthrowable obj) {
            return functions->Throw(this, obj);
        }
        jint ThrowNew(jclass clazz, const char* msg) {
            return functions->ThrowNew(this, clazz, msg);
        }
        jthrowable ExceptionOccurred() {
            return functions->ExceptionOccurred(this);
        }
        void ExceptionDescribe() {
            functions->ExceptionDescribe(this);
        }
        void ExceptionClear() {
            functions->ExceptionClear(this);
        }
        void FatalError(const char* msg) {
            functions->FatalError(this, msg);
        }

        jint PushLocalFrame(jint capacity) {
            return functions->PushLocalFrame(this, capacity);
        }
        jobject PopLocalFrame(jobject result) {
            return functions->PopLocalFrame(this, result);
        }

        jobject NewGlobalRef(jobject lobj) {
            return functions->NewGlobalRef(this, lobj);
        }
        void DeleteGlobalRef(jobject gref) {
            functions->DeleteGlobalRef(this, gref);
        }
        void DeleteLocalRef(jobject obj) {
            functions->DeleteLocalRef(this, obj);
        }

        jboolean IsSameObject(jobject obj1, jobject obj2) {
            return functions->IsSameObject(this, obj1, obj2);
        }

        jobject NewLocalRef(jobject ref) {
            return functions->NewLocalRef(this, ref);
        }
        jint EnsureLocalCapacity(jint capacity) {
            return functions->EnsureLocalCapacity(this, capacity);
        }

        jobject AllocObject(jclass clazz) {
            return functions->AllocObject(this, clazz);
        }
        jobject NewObject(jclass clazz, jmethodID methodID, ...) {
            va_list args;
            jobject result;
            va_start(args, methodID);
            result = functions->NewObjectV(this, clazz, methodID, args);
            va_end(args);
            return result;
        }
        jobject NewObjectV(jclass clazz, jmethodID methodID,
            va_list args) {
            return functions->NewObjectV(this, clazz, methodID, args);
        }
        jobject NewObjectA(jclass clazz, jmethodID methodID,
            const jvalue* args) {
            return functions->NewObjectA(this, clazz, methodID, args);
        }

        jclass GetObjectClass(jobject obj) {
            return functions->GetObjectClass(this, obj);
        }
        jboolean IsInstanceOf(jobject obj, jclass clazz) {
            return functions->IsInstanceOf(this, obj, clazz);
        }

        jmethodID GetMethodID(jclass clazz, const char* name,
            const char* sig) {
            return functions->GetMethodID(this, clazz, name, sig);
        }

        jobject CallObjectMethod(jobject obj, jmethodID methodID, ...) {
            va_list args;
            jobject result;
            va_start(args, methodID);
            result = functions->CallObjectMethodV(this, obj, methodID, args);
            va_end(args);
            return result;
        }
        jobject CallObjectMethodV(jobject obj, jmethodID methodID,
            va_list args) {
            return functions->CallObjectMethodV(this, obj, methodID, args);
        }
        jobject CallObjectMethodA(jobject obj, jmethodID methodID,
            const jvalue* args) {
            return functions->CallObjectMethodA(this, obj, methodID, args);
        }

        jboolean CallBooleanMethod(jobject obj,
            jmethodID methodID, ...) {
            va_list args;
            jboolean result;
            va_start(args, methodID);
            result = functions->CallBooleanMethodV(this, obj, methodID, args);
            va_end(args);
            return result;
        }
        jboolean CallBooleanMethodV(jobject obj, jmethodID methodID,
            va_list args) {
            return functions->CallBooleanMethodV(this, obj, methodID, args);
        }
        jboolean CallBooleanMethodA(jobject obj, jmethodID methodID,
            const jvalue* args) {
            return functions->CallBooleanMethodA(this, obj, methodID, args);
        }

        jbyte CallByteMethod(jobject obj, jmethodID methodID, ...) {
            va_list args;
            jbyte result;
            va_start(args, methodID);
            result = functions->CallByteMethodV(this, obj, methodID, args);
            va_end(args);
            return result;
        }
        jbyte CallByteMethodV(jobject obj, jmethodID methodID,
            va_list args) {
            return functions->CallByteMethodV(this, obj, methodID, args);
        }
        jbyte CallByteMethodA(jobject obj, jmethodID methodID,
            const jvalue* args) {
            return functions->CallByteMethodA(this, obj, methodID, args);
        }

        jchar CallCharMethod(jobject obj, jmethodID methodID, ...) {
            va_list args;
            jchar result;
            va_start(args, methodID);
            result = functions->CallCharMethodV(this, obj, methodID, args);
            va_end(args);
            return result;
        }
        jchar CallCharMethodV(jobject obj, jmethodID methodID,
            va_list args) {
            return functions->CallCharMethodV(this, obj, methodID, args);
        }
        jchar CallCharMethodA(jobject obj, jmethodID methodID,
            const jvalue* args) {
            return functions->CallCharMethodA(this, obj, methodID, args);
        }

        jshort CallShortMethod(jobject obj, jmethodID methodID, ...) {
            va_list args;
            jshort result;
            va_start(args, methodID);
            result = functions->CallShortMethodV(this, obj, methodID, args);
            va_end(args);
            return result;
        }
        jshort CallShortMethodV(jobject obj, jmethodID methodID,
            va_list args) {
            return functions->CallShortMethodV(this, obj, methodID, args);
        }
        jshort CallShortMethodA(jobject obj, jmethodID methodID,
            const jvalue* args) {
            return functions->CallShortMethodA(this, obj, methodID, args);
        }

        jint CallIntMethod(jobject obj, jmethodID methodID, ...) {
            va_list args;
            jint result;
            va_start(args, methodID);
            result = functions->CallIntMethodV(this, obj, methodID, args);
            va_end(args);
            return result;
        }
        jint CallIntMethodV(jobject obj, jmethodID methodID,
            va_list args) {
            return functions->CallIntMethodV(this, obj, methodID, args);
        }
        jint CallIntMethodA(jobject obj, jmethodID methodID,
            const jvalue* args) {
            return functions->CallIntMethodA(this, obj, methodID, args);
        }

        jlong CallLongMethod(jobject obj, jmethodID methodID, ...) {
            va_list args;
            jlong result;
            va_start(args, methodID);
            result = functions->CallLongMethodV(this, obj, methodID, args);
            va_end(args);
            return result;
        }
        jlong CallLongMethodV(jobject obj, jmethodID methodID,
            va_list args) {
            return functions->CallLongMethodV(this, obj, methodID, args);
        }
        jlong CallLongMethodA(jobject obj, jmethodID methodID,
            const jvalue* args) {
            return functions->CallLongMethodA(this, obj, methodID, args);
        }

        jfloat CallFloatMethod(jobject obj, jmethodID methodID, ...) {
            va_list args;
            jfloat result;
            va_start(args, methodID);
            result = functions->CallFloatMethodV(this, obj, methodID, args);
            va_end(args);
            return result;
        }
        jfloat CallFloatMethodV(jobject obj, jmethodID methodID,
            va_list args) {
            return functions->CallFloatMethodV(this, obj, methodID, args);
        }
        jfloat CallFloatMethodA(jobject obj, jmethodID methodID,
            const jvalue* args) {
            return functions->CallFloatMethodA(this, obj, methodID, args);
        }

        jdouble CallDoubleMethod(jobject obj, jmethodID methodID, ...) {
            va_list args;
            jdouble result;
            va_start(args, methodID);
            result = functions->CallDoubleMethodV(this, obj, methodID, args);
            va_end(args);
            return result;
        }
        jdouble CallDoubleMethodV(jobject obj, jmethodID methodID,
            va_list args) {
            return functions->CallDoubleMethodV(this, obj, methodID, args);
        }
        jdouble CallDoubleMethodA(jobject obj, jmethodID methodID,
            const jvalue* args) {
            return functions->CallDoubleMethodA(this, obj, methodID, args);
        }

        void CallVoidMethod(jobject obj, jmethodID methodID, ...) {
            va_list args;
            va_start(args, methodID);
            functions->CallVoidMethodV(this, obj, methodID, args);
            va_end(args);
        }
        void CallVoidMethodV(jobject obj, jmethodID methodID,
            va_list args) {
            functions->CallVoidMethodV(this, obj, methodID, args);
        }
        void CallVoidMethodA(jobject obj, jmethodID methodID,
            const jvalue* args) {
            functions->CallVoidMethodA(this, obj, methodID, args);
        }

        jobject CallNonvirtualObjectMethod(jobject obj, jclass clazz,
            jmethodID methodID, ...) {
            va_list args;
            jobject result;
            va_start(args, methodID);
            result = functions->CallNonvirtualObjectMethodV(this, obj, clazz,
                methodID, args);
            va_end(args);
            return result;
        }
        jobject CallNonvirtualObjectMethodV(jobject obj, jclass clazz,
            jmethodID methodID, va_list args) {
            return functions->CallNonvirtualObjectMethodV(this, obj, clazz,
                methodID, args);
        }
        jobject CallNonvirtualObjectMethodA(jobject obj, jclass clazz,
            jmethodID methodID, const jvalue* args) {
            return functions->CallNonvirtualObjectMethodA(this, obj, clazz,
                methodID, args);
        }

        jboolean CallNonvirtualBooleanMethod(jobject obj, jclass clazz,
            jmethodID methodID, ...) {
            va_list args;
            jboolean result;
            va_start(args, methodID);
            result = functions->CallNonvirtualBooleanMethodV(this, obj, clazz,
                methodID, args);
            va_end(args);
            return result;
        }
        jboolean CallNonvirtualBooleanMethodV(jobject obj, jclass clazz,
            jmethodID methodID, va_list args) {
            return functions->CallNonvirtualBooleanMethodV(this, obj, clazz,
                methodID, args);
        }
        jboolean CallNonvirtualBooleanMethodA(jobject obj, jclass clazz,
            jmethodID methodID, const jvalue* args) {
            return functions->CallNonvirtualBooleanMethodA(this, obj, clazz,
                methodID, args);
        }

        jbyte CallNonvirtualByteMethod(jobject obj, jclass clazz,
            jmethodID methodID, ...) {
            va_list args;
            jbyte result;
            va_start(args, methodID);
            result = functions->CallNonvirtualByteMethodV(this, obj, clazz,
                methodID, args);
            va_end(args);
            return result;
        }
        jbyte CallNonvirtualByteMethodV(jobject obj, jclass clazz,
            jmethodID methodID, va_list args) {
            return functions->CallNonvirtualByteMethodV(this, obj, clazz,
                methodID, args);
        }
        jbyte CallNonvirtualByteMethodA(jobject obj, jclass clazz,
            jmethodID methodID, const jvalue* args) {
            return functions->CallNonvirtualByteMethodA(this, obj, clazz,
                methodID, args);
        }

        jchar CallNonvirtualCharMethod(jobject obj, jclass clazz,
            jmethodID methodID, ...) {
            va_list args;
            jchar result;
            va_start(args, methodID);
            result = functions->CallNonvirtualCharMethodV(this, obj, clazz,
                methodID, args);
            va_end(args);
            return result;
        }
        jchar CallNonvirtualCharMethodV(jobject obj, jclass clazz,
            jmethodID methodID, va_list args) {
            return functions->CallNonvirtualCharMethodV(this, obj, clazz,
                methodID, args);
        }
        jchar CallNonvirtualCharMethodA(jobject obj, jclass clazz,
            jmethodID methodID, const jvalue* args) {
            return functions->CallNonvirtualCharMethodA(this, obj, clazz,
                methodID, args);
        }

        jshort CallNonvirtualShortMethod(jobject obj, jclass clazz,
            jmethodID methodID, ...) {
            va_list args;
            jshort result;
            va_start(args, methodID);
            result = functions->CallNonvirtualShortMethodV(this, obj, clazz,
                methodID, args);
            va_end(args);
            return result;
        }
        jshort CallNonvirtualShortMethodV(jobject obj, jclass clazz,
            jmethodID methodID, va_list args) {
            return functions->CallNonvirtualShortMethodV(this, obj, clazz,
                methodID, args);
        }
        jshort CallNonvirtualShortMethodA(jobject obj, jclass clazz,
            jmethodID methodID, const jvalue* args) {
            return functions->CallNonvirtualShortMethodA(this, obj, clazz,
                methodID, args);
        }

        jint CallNonvirtualIntMethod(jobject obj, jclass clazz,
            jmethodID methodID, ...) {
            va_list args;
            jint result;
            va_start(args, methodID);
            result = functions->CallNonvirtualIntMethodV(this, obj, clazz,
                methodID, args);
            va_end(args);
            return result;
        }
        jint CallNonvirtualIntMethodV(jobject obj, jclass clazz,
            jmethodID methodID, va_list args) {
            return functions->CallNonvirtualIntMethodV(this, obj, clazz,
                methodID, args);
        }
        jint CallNonvirtualIntMethodA(jobject obj, jclass clazz,
            jmethodID methodID, const jvalue* args) {
            return functions->CallNonvirtualIntMethodA(this, obj, clazz,
                methodID, args);
        }

        jlong CallNonvirtualLongMethod(jobject obj, jclass clazz,
            jmethodID methodID, ...) {
            va_list args;
            jlong result;
            va_start(args, methodID);
            result = functions->CallNonvirtualLongMethodV(this, obj, clazz,
                methodID, args);
            va_end(args);
            return result;
        }
        jlong CallNonvirtualLongMethodV(jobject obj, jclass clazz,
            jmethodID methodID, va_list args) {
            return functions->CallNonvirtualLongMethodV(this, obj, clazz,
                methodID, args);
        }
        jlong CallNonvirtualLongMethodA(jobject obj, jclass clazz,
            jmethodID methodID, const jvalue* args) {
            return functions->CallNonvirtualLongMethodA(this, obj, clazz,
                methodID, args);
        }

        jfloat CallNonvirtualFloatMethod(jobject obj, jclass clazz,
            jmethodID methodID, ...) {
            va_list args;
            jfloat result;
            va_start(args, methodID);
            result = functions->CallNonvirtualFloatMethodV(this, obj, clazz,
                methodID, args);
            va_end(args);
            return result;
        }
        jfloat CallNonvirtualFloatMethodV(jobject obj, jclass clazz,
            jmethodID methodID,
            va_list args) {
            return functions->CallNonvirtualFloatMethodV(this, obj, clazz,
                methodID, args);
        }
        jfloat CallNonvirtualFloatMethodA(jobject obj, jclass clazz,
            jmethodID methodID,
            const jvalue* args) {
            return functions->CallNonvirtualFloatMethodA(this, obj, clazz,
                methodID, args);
        }

        jdouble CallNonvirtualDoubleMethod(jobject obj, jclass clazz,
            jmethodID methodID, ...) {
            va_list args;
            jdouble result;
            va_start(args, methodID);
            result = functions->CallNonvirtualDoubleMethodV(this, obj, clazz,
                methodID, args);
            va_end(args);
            return result;
        }
        jdouble CallNonvirtualDoubleMethodV(jobject obj, jclass clazz,
            jmethodID methodID,
            va_list args) {
            return functions->CallNonvirtualDoubleMethodV(this, obj, clazz,
                methodID, args);
        }
        jdouble CallNonvirtualDoubleMethodA(jobject obj, jclass clazz,
            jmethodID methodID,
            const jvalue* args) {
            return functions->CallNonvirtualDoubleMethodA(this, obj, clazz,
                methodID, args);
        }

        void CallNonvirtualVoidMethod(jobject obj, jclass clazz,
            jmethodID methodID, ...) {
            va_list args;
            va_start(args, methodID);
            functions->CallNonvirtualVoidMethodV(this, obj, clazz, methodID, args);
            va_end(args);
        }
        void CallNonvirtualVoidMethodV(jobject obj, jclass clazz,
            jmethodID methodID,
            va_list args) {
            functions->CallNonvirtualVoidMethodV(this, obj, clazz, methodID, args);
        }
        void CallNonvirtualVoidMethodA(jobject obj, jclass clazz,
            jmethodID methodID,
            const jvalue* args) {
            functions->CallNonvirtualVoidMethodA(this, obj, clazz, methodID, args);
        }

        jfieldID GetFieldID(jclass clazz, const char* name,
            const char* sig) {
            return functions->GetFieldID(this, clazz, name, sig);
        }

        jobject GetObjectField(jobject obj, jfieldID fieldID) {
            return functions->GetObjectField(this, obj, fieldID);
        }
        jboolean GetBooleanField(jobject obj, jfieldID fieldID) {
            return functions->GetBooleanField(this, obj, fieldID);
        }
        jbyte GetByteField(jobject obj, jfieldID fieldID) {
            return functions->GetByteField(this, obj, fieldID);
        }
        jchar GetCharField(jobject obj, jfieldID fieldID) {
            return functions->GetCharField(this, obj, fieldID);
        }
        jshort GetShortField(jobject obj, jfieldID fieldID) {
            return functions->GetShortField(this, obj, fieldID);
        }
        jint GetIntField(jobject obj, jfieldID fieldID) {
            return functions->GetIntField(this, obj, fieldID);
        }
        jlong GetLongField(jobject obj, jfieldID fieldID) {
            return functions->GetLongField(this, obj, fieldID);
        }
        jfloat GetFloatField(jobject obj, jfieldID fieldID) {
            return functions->GetFloatField(this, obj, fieldID);
        }
        jdouble GetDoubleField(jobject obj, jfieldID fieldID) {
            return functions->GetDoubleField(this, obj, fieldID);
        }

        void SetObjectField(jobject obj, jfieldID fieldID, jobject val) {
            functions->SetObjectField(this, obj, fieldID, val);
        }
        void SetBooleanField(jobject obj, jfieldID fieldID,
            jboolean val) {
            functions->SetBooleanField(this, obj, fieldID, val);
        }
        void SetByteField(jobject obj, jfieldID fieldID,
            jbyte val) {
            functions->SetByteField(this, obj, fieldID, val);
        }
        void SetCharField(jobject obj, jfieldID fieldID,
            jchar val) {
            functions->SetCharField(this, obj, fieldID, val);
        }
        void SetShortField(jobject obj, jfieldID fieldID,
            jshort val) {
            functions->SetShortField(this, obj, fieldID, val);
        }
        void SetIntField(jobject obj, jfieldID fieldID,
            jint val) {
            functions->SetIntField(this, obj, fieldID, val);
        }
        void SetLongField(jobject obj, jfieldID fieldID,
            jlong val) {
            functions->SetLongField(this, obj, fieldID, val);
        }
        void SetFloatField(jobject obj, jfieldID fieldID,
            jfloat val) {
            functions->SetFloatField(this, obj, fieldID, val);
        }
        void SetDoubleField(jobject obj, jfieldID fieldID,
            jdouble val) {
            functions->SetDoubleField(this, obj, fieldID, val);
        }

        jmethodID GetStaticMethodID(jclass clazz, const char* name,
            const char* sig) {
            return functions->GetStaticMethodID(this, clazz, name, sig);
        }

        jobject CallStaticObjectMethod(jclass clazz, jmethodID methodID,
            ...) {
            va_list args;
            jobject result;
            va_start(args, methodID);
            result = functions->CallStaticObjectMethodV(this, clazz, methodID, args);
            va_end(args);
            return result;
        }
        jobject CallStaticObjectMethodV(jclass clazz, jmethodID methodID,
            va_list args) {
            return functions->CallStaticObjectMethodV(this, clazz, methodID, args);
        }
        jobject CallStaticObjectMethodA(jclass clazz, jmethodID methodID,
            const jvalue* args) {
            return functions->CallStaticObjectMethodA(this, clazz, methodID, args);
        }

        jboolean CallStaticBooleanMethod(jclass clazz,
            jmethodID methodID, ...) {
            va_list args;
            jboolean result;
            va_start(args, methodID);
            result = functions->CallStaticBooleanMethodV(this, clazz, methodID, args);
            va_end(args);
            return result;
        }
        jboolean CallStaticBooleanMethodV(jclass clazz,
            jmethodID methodID, va_list args) {
            return functions->CallStaticBooleanMethodV(this, clazz, methodID, args);
        }
        jboolean CallStaticBooleanMethodA(jclass clazz,
            jmethodID methodID, const jvalue* args) {
            return functions->CallStaticBooleanMethodA(this, clazz, methodID, args);
        }

        jbyte CallStaticByteMethod(jclass clazz,
            jmethodID methodID, ...) {
            va_list args;
            jbyte result;
            va_start(args, methodID);
            result = functions->CallStaticByteMethodV(this, clazz, methodID, args);
            va_end(args);
            return result;
        }
        jbyte CallStaticByteMethodV(jclass clazz,
            jmethodID methodID, va_list args) {
            return functions->CallStaticByteMethodV(this, clazz, methodID, args);
        }
        jbyte CallStaticByteMethodA(jclass clazz,
            jmethodID methodID, const jvalue* args) {
            return functions->CallStaticByteMethodA(this, clazz, methodID, args);
        }

        jchar CallStaticCharMethod(jclass clazz,
            jmethodID methodID, ...) {
            va_list args;
            jchar result;
            va_start(args, methodID);
            result = functions->CallStaticCharMethodV(this, clazz, methodID, args);
            va_end(args);
            return result;
        }
        jchar CallStaticCharMethodV(jclass clazz,
            jmethodID methodID, va_list args) {
            return functions->CallStaticCharMethodV(this, clazz, methodID, args);
        }
        jchar CallStaticCharMethodA(jclass clazz,
            jmethodID methodID, const jvalue* args) {
            return functions->CallStaticCharMethodA(this, clazz, methodID, args);
        }

        jshort CallStaticShortMethod(jclass clazz,
            jmethodID methodID, ...) {
            va_list args;
            jshort result;
            va_start(args, methodID);
            result = functions->CallStaticShortMethodV(this, clazz, methodID, args);
            va_end(args);
            return result;
        }
        jshort CallStaticShortMethodV(jclass clazz,
            jmethodID methodID, va_list args) {
            return functions->CallStaticShortMethodV(this, clazz, methodID, args);
        }
        jshort CallStaticShortMethodA(jclass clazz,
            jmethodID methodID, const jvalue* args) {
            return functions->CallStaticShortMethodA(this, clazz, methodID, args);
        }

        jint CallStaticIntMethod(jclass clazz,
            jmethodID methodID, ...) {
            va_list args;
            jint result;
            va_start(args, methodID);
            result = functions->CallStaticIntMethodV(this, clazz, methodID, args);
            va_end(args);
            return result;
        }
        jint CallStaticIntMethodV(jclass clazz,
            jmethodID methodID, va_list args) {
            return functions->CallStaticIntMethodV(this, clazz, methodID, args);
        }
        jint CallStaticIntMethodA(jclass clazz,
            jmethodID methodID, const jvalue* args) {
            return functions->CallStaticIntMethodA(this, clazz, methodID, args);
        }

        jlong CallStaticLongMethod(jclass clazz,
            jmethodID methodID, ...) {
            va_list args;
            jlong result;
            va_start(args, methodID);
            result = functions->CallStaticLongMethodV(this, clazz, methodID, args);
            va_end(args);
            return result;
        }
        jlong CallStaticLongMethodV(jclass clazz,
            jmethodID methodID, va_list args) {
            return functions->CallStaticLongMethodV(this, clazz, methodID, args);
        }
        jlong CallStaticLongMethodA(jclass clazz,
            jmethodID methodID, const jvalue* args) {
            return functions->CallStaticLongMethodA(this, clazz, methodID, args);
        }

        jfloat CallStaticFloatMethod(jclass clazz,
            jmethodID methodID, ...) {
            va_list args;
            jfloat result;
            va_start(args, methodID);
            result = functions->CallStaticFloatMethodV(this, clazz, methodID, args);
            va_end(args);
            return result;
        }
        jfloat CallStaticFloatMethodV(jclass clazz,
            jmethodID methodID, va_list args) {
            return functions->CallStaticFloatMethodV(this, clazz, methodID, args);
        }
        jfloat CallStaticFloatMethodA(jclass clazz,
            jmethodID methodID, const jvalue* args) {
            return functions->CallStaticFloatMethodA(this, clazz, methodID, args);
        }

        jdouble CallStaticDoubleMethod(jclass clazz,
            jmethodID methodID, ...) {
            va_list args;
            jdouble result;
            va_start(args, methodID);
            result = functions->CallStaticDoubleMethodV(this, clazz, methodID, args);
            va_end(args);
            return result;
        }
        jdouble CallStaticDoubleMethodV(jclass clazz,
            jmethodID methodID, va_list args) {
            return functions->CallStaticDoubleMethodV(this, clazz, methodID, args);
        }
        jdouble CallStaticDoubleMethodA(jclass clazz,
            jmethodID methodID, const jvalue* args) {
            return functions->CallStaticDoubleMethodA(this, clazz, methodID, args);
        }

        void CallStaticVoidMethod(jclass cls, jmethodID methodID, ...) {
            va_list args;
            va_start(args, methodID);
            functions->CallStaticVoidMethodV(this, cls, methodID, args);
            va_end(args);
        }
        void CallStaticVoidMethodV(jclass cls, jmethodID methodID,
            va_list args) {
            functions->CallStaticVoidMethodV(this, cls, methodID, args);
        }
        void CallStaticVoidMethodA(jclass cls, jmethodID methodID,
            const jvalue* args) {
            functions->CallStaticVoidMethodA(this, cls, methodID, args);
        }

        jfieldID GetStaticFieldID(jclass clazz, const char* name,
            const char* sig) {
            return functions->GetStaticFieldID(this, clazz, name, sig);
        }
        jobject GetStaticObjectField(jclass clazz, jfieldID fieldID) {
            return functions->GetStaticObjectField(this, clazz, fieldID);
        }
        jboolean GetStaticBooleanField(jclass clazz, jfieldID fieldID) {
            return functions->GetStaticBooleanField(this, clazz, fieldID);
        }
        jbyte GetStaticByteField(jclass clazz, jfieldID fieldID) {
            return functions->GetStaticByteField(this, clazz, fieldID);
        }
        jchar GetStaticCharField(jclass clazz, jfieldID fieldID) {
            return functions->GetStaticCharField(this, clazz, fieldID);
        }
        jshort GetStaticShortField(jclass clazz, jfieldID fieldID) {
            return functions->GetStaticShortField(this, clazz, fieldID);
        }
        jint GetStaticIntField(jclass clazz, jfieldID fieldID) {
            return functions->GetStaticIntField(this, clazz, fieldID);
        }
        jlong GetStaticLongField(jclass clazz, jfieldID fieldID) {
            return functions->GetStaticLongField(this, clazz, fieldID);
        }
        jfloat GetStaticFloatField(jclass clazz, jfieldID fieldID) {
            return functions->GetStaticFloatField(this, clazz, fieldID);
        }
        jdouble GetStaticDoubleField(jclass clazz, jfieldID fieldID) {
            return functions->GetStaticDoubleField(this, clazz, fieldID);
        }

        void SetStaticObjectField(jclass clazz, jfieldID fieldID,
            jobject value) {
            functions->SetStaticObjectField(this, clazz, fieldID, value);
        }
        void SetStaticBooleanField(jclass clazz, jfieldID fieldID,
            jboolean value) {
            functions->SetStaticBooleanField(this, clazz, fieldID, value);
        }
        void SetStaticByteField(jclass clazz, jfieldID fieldID,
            jbyte value) {
            functions->SetStaticByteField(this, clazz, fieldID, value);
        }
        void SetStaticCharField(jclass clazz, jfieldID fieldID,
            jchar value) {
            functions->SetStaticCharField(this, clazz, fieldID, value);
        }
        void SetStaticShortField(jclass clazz, jfieldID fieldID,
            jshort value) {
            functions->SetStaticShortField(this, clazz, fieldID, value);
        }
        void SetStaticIntField(jclass clazz, jfieldID fieldID,
            jint value) {
            functions->SetStaticIntField(this, clazz, fieldID, value);
        }
        void SetStaticLongField(jclass clazz, jfieldID fieldID,
            jlong value) {
            functions->SetStaticLongField(this, clazz, fieldID, value);
        }
        void SetStaticFloatField(jclass clazz, jfieldID fieldID,
            jfloat value) {
            functions->SetStaticFloatField(this, clazz, fieldID, value);
        }
        void SetStaticDoubleField(jclass clazz, jfieldID fieldID,
            jdouble value) {
            functions->SetStaticDoubleField(this, clazz, fieldID, value);
        }

        jstring NewString(const jchar* unicode, jsize len) {
            return functions->NewString(this, unicode, len);
        }
        jsize GetStringLength(jstring str) {
            return functions->GetStringLength(this, str);
        }
        const jchar* GetStringChars(jstring str, jboolean* isCopy) {
            return functions->GetStringChars(this, str, isCopy);
        }
        void ReleaseStringChars(jstring str, const jchar* chars) {
            functions->ReleaseStringChars(this, str, chars);
        }

        jstring NewStringUTF(const char* utf) {
            return functions->NewStringUTF(this, utf);
        }
        jsize GetStringUTFLength(jstring str) {
            return functions->GetStringUTFLength(this, str);
        }
        const char* GetStringUTFChars(jstring str, jboolean* isCopy) {
            return functions->GetStringUTFChars(this, str, isCopy);
        }
        void ReleaseStringUTFChars(jstring str, const char* chars) {
            functions->ReleaseStringUTFChars(this, str, chars);
        }

        jsize GetArrayLength(jarray array) {
            return functions->GetArrayLength(this, array);
        }

        jobjectArray NewObjectArray(jsize len, jclass clazz,
            jobject init) {
            return functions->NewObjectArray(this, len, clazz, init);
        }
        jobject GetObjectArrayElement(jobjectArray array, jsize index) {
            return functions->GetObjectArrayElement(this, array, index);
        }
        void SetObjectArrayElement(jobjectArray array, jsize index,
            jobject val) {
            functions->SetObjectArrayElement(this, array, index, val);
        }

        jbooleanArray NewBooleanArray(jsize len) {
            return functions->NewBooleanArray(this, len);
        }
        jbyteArray NewByteArray(jsize len) {
            return functions->NewByteArray(this, len);
        }
        jcharArray NewCharArray(jsize len) {
            return functions->NewCharArray(this, len);
        }
        jshortArray NewShortArray(jsize len) {
            return functions->NewShortArray(this, len);
        }
        jintArray NewIntArray(jsize len) {
            return functions->NewIntArray(this, len);
        }
        jlongArray NewLongArray(jsize len) {
            return functions->NewLongArray(this, len);
        }
        jfloatArray NewFloatArray(jsize len) {
            return functions->NewFloatArray(this, len);
        }
        jdoubleArray NewDoubleArray(jsize len) {
            return functions->NewDoubleArray(this, len);
        }

        jboolean* GetBooleanArrayElements(jbooleanArray array, jboolean* isCopy) {
            return functions->GetBooleanArrayElements(this, array, isCopy);
        }
        jbyte* GetByteArrayElements(jbyteArray array, jboolean* isCopy) {
            return functions->GetByteArrayElements(this, array, isCopy);
        }
        jchar* GetCharArrayElements(jcharArray array, jboolean* isCopy) {
            return functions->GetCharArrayElements(this, array, isCopy);
        }
        jshort* GetShortArrayElements(jshortArray array, jboolean* isCopy) {
            return functions->GetShortArrayElements(this, array, isCopy);
        }
        jint* GetIntArrayElements(jintArray array, jboolean* isCopy) {
            return functions->GetIntArrayElements(this, array, isCopy);
        }
        jlong* GetLongArrayElements(jlongArray array, jboolean* isCopy) {
            return functions->GetLongArrayElements(this, array, isCopy);
        }
        jfloat* GetFloatArrayElements(jfloatArray array, jboolean* isCopy) {
            return functions->GetFloatArrayElements(this, array, isCopy);
        }
        jdouble* GetDoubleArrayElements(jdoubleArray array, jboolean* isCopy) {
            return functions->GetDoubleArrayElements(this, array, isCopy);
        }

        void ReleaseBooleanArrayElements(jbooleanArray array,
            jboolean* elems,
            jint mode) {
            functions->ReleaseBooleanArrayElements(this, array, elems, mode);
        }
        void ReleaseByteArrayElements(jbyteArray array,
            jbyte* elems,
            jint mode) {
            functions->ReleaseByteArrayElements(this, array, elems, mode);
        }
        void ReleaseCharArrayElements(jcharArray array,
            jchar* elems,
            jint mode) {
            functions->ReleaseCharArrayElements(this, array, elems, mode);
        }
        void ReleaseShortArrayElements(jshortArray array,
            jshort* elems,
            jint mode) {
            functions->ReleaseShortArrayElements(this, array, elems, mode);
        }
        void ReleaseIntArrayElements(jintArray array,
            jint* elems,
            jint mode) {
            functions->ReleaseIntArrayElements(this, array, elems, mode);
        }
        void ReleaseLongArrayElements(jlongArray array,
            jlong* elems,
            jint mode) {
            functions->ReleaseLongArrayElements(this, array, elems, mode);
        }
        void ReleaseFloatArrayElements(jfloatArray array,
            jfloat* elems,
            jint mode) {
            functions->ReleaseFloatArrayElements(this, array, elems, mode);
        }
        void ReleaseDoubleArrayElements(jdoubleArray array,
            jdouble* elems,
            jint mode) {
            functions->ReleaseDoubleArrayElements(this, array, elems, mode);
        }

        void GetBooleanArrayRegion(jbooleanArray array,
            jsize start, jsize len, jboolean* buf) {
            functions->GetBooleanArrayRegion(this, array, start, len, buf);
        }
        void GetByteArrayRegion(jbyteArray array,
            jsize start, jsize len, jbyte* buf) {
            functions->GetByteArrayRegion(this, array, start, len, buf);
        }
        void GetCharArrayRegion(jcharArray array,
            jsize start, jsize len, jchar* buf) {
            functions->GetCharArrayRegion(this, array, start, len, buf);
        }
        void GetShortArrayRegion(jshortArray array,
            jsize start, jsize len, jshort* buf) {
            functions->GetShortArrayRegion(this, array, start, len, buf);
        }
        void GetIntArrayRegion(jintArray array,
            jsize start, jsize len, jint* buf) {
            functions->GetIntArrayRegion(this, array, start, len, buf);
        }
        void GetLongArrayRegion(jlongArray array,
            jsize start, jsize len, jlong* buf) {
            functions->GetLongArrayRegion(this, array, start, len, buf);
        }
        void GetFloatArrayRegion(jfloatArray array,
            jsize start, jsize len, jfloat* buf) {
            functions->GetFloatArrayRegion(this, array, start, len, buf);
        }
        void GetDoubleArrayRegion(jdoubleArray array,
            jsize start, jsize len, jdouble* buf) {
            functions->GetDoubleArrayRegion(this, array, start, len, buf);
        }

        void SetBooleanArrayRegion(jbooleanArray array, jsize start, jsize len,
            const jboolean* buf) {
            functions->SetBooleanArrayRegion(this, array, start, len, buf);
        }
        void SetByteArrayRegion(jbyteArray array, jsize start, jsize len,
            const jbyte* buf) {
            functions->SetByteArrayRegion(this, array, start, len, buf);
        }
        void SetCharArrayRegion(jcharArray array, jsize start, jsize len,
            const jchar* buf) {
            functions->SetCharArrayRegion(this, array, start, len, buf);
        }
        void SetShortArrayRegion(jshortArray array, jsize start, jsize len,
            const jshort* buf) {
            functions->SetShortArrayRegion(this, array, start, len, buf);
        }
        void SetIntArrayRegion(jintArray array, jsize start, jsize len,
            const jint* buf) {
            functions->SetIntArrayRegion(this, array, start, len, buf);
        }
        void SetLongArrayRegion(jlongArray array, jsize start, jsize len,
            const jlong* buf) {
            functions->SetLongArrayRegion(this, array, start, len, buf);
        }
        void SetFloatArrayRegion(jfloatArray array, jsize start, jsize len,
            const jfloat* buf) {
            functions->SetFloatArrayRegion(this, array, start, len, buf);
        }
        void SetDoubleArrayRegion(jdoubleArray array, jsize start, jsize len,
            const jdouble* buf) {
            functions->SetDoubleArrayRegion(this, array, start, len, buf);
        }

        jint RegisterNatives(jclass clazz, const JNINativeMethod* methods,
            jint nMethods) {
            return functions->RegisterNatives(this, clazz, methods, nMethods);
        }
        jint UnregisterNatives(jclass clazz) {
            return functions->UnregisterNatives(this, clazz);
        }

        jint MonitorEnter(jobject obj) {
            return functions->MonitorEnter(this, obj);
        }
        jint MonitorExit(jobject obj) {
            return functions->MonitorExit(this, obj);
        }

        jint GetJavaVM(JavaVM** vm) {
            return functions->GetJavaVM(this, vm);
        }

        void GetStringRegion(jstring str, jsize start, jsize len, jchar* buf) {
            functions->GetStringRegion(this, str, start, len, buf);
        }
        void GetStringUTFRegion(jstring str, jsize start, jsize len, char* buf) {
            functions->GetStringUTFRegion(this, str, start, len, buf);
        }

        void* GetPrimitiveArrayCritical(jarray array, jboolean* isCopy) {
            return functions->GetPrimitiveArrayCritical(this, array, isCopy);
        }
        void ReleasePrimitiveArrayCritical(jarray array, void* carray, jint mode) {
            functions->ReleasePrimitiveArrayCritical(this, array, carray, mode);
        }

        const jchar* GetStringCritical(jstring string, jboolean* isCopy) {
            return functions->GetStringCritical(this, string, isCopy);
        }
        void ReleaseStringCritical(jstring string, const jchar* cstring) {
            functions->ReleaseStringCritical(this, string, cstring);
        }

        jweak NewWeakGlobalRef(jobject obj) {
            return functions->NewWeakGlobalRef(this, obj);
        }
        void DeleteWeakGlobalRef(jweak ref) {
            functions->DeleteWeakGlobalRef(this, ref);
        }

        jboolean ExceptionCheck() {
            return functions->ExceptionCheck(this);
        }

        jobject NewDirectByteBuffer(void* address, jlong capacity) {
            return functions->NewDirectByteBuffer(this, address, capacity);
        }
        void* GetDirectBufferAddress(jobject buf) {
            return functions->GetDirectBufferAddress(this, buf);
        }
        jlong GetDirectBufferCapacity(jobject buf) {
            return functions->GetDirectBufferCapacity(this, buf);
        }
        jobjectRefType GetObjectRefType(jobject obj) {
            return functions->GetObjectRefType(this, obj);
        }

        /* Module Features */

        jobject GetModule(jclass clazz) {
            return functions->GetModule(this, clazz);
        }

#endif /* __cplusplus */
    };

    /*
     * optionString may be any option accepted by the JVM, or one of the
     * following:
     *
     * -D<name>=<value>          Set a system property.
     * -verbose[:class|gc|jni]   Enable verbose output, comma-separated. E.g.
     *                           "-verbose:class" or "-verbose:gc,class"
     *                           Standard names include: gc, class, and jni.
     *                           All nonstandard (VM-specific) names must begin
     *                           with "X".
     * vfprintf                  extraInfo is a pointer to the vfprintf hook.
     * exit                      extraInfo is a pointer to the exit hook.
     * abort                     extraInfo is a pointer to the abort hook.
     */
    typedef struct JavaVMOption {
        char* optionString;
        void* extraInfo;
    } JavaVMOption;

    typedef struct JavaVMInitArgs {
        jint version;

        jint nOptions;
        JavaVMOption* options;
        jboolean ignoreUnrecognized;
    } JavaVMInitArgs;

    typedef struct JavaVMAttachArgs {
        jint version;

        char* name;
        jobject group;
    } JavaVMAttachArgs;

    /* These will be VM-specific. */

#define JDK1_2
#define JDK1_4

/* End VM-specific. */

    struct JNIInvokeInterface_ {
        void* reserved0;
        void* reserved1;
        void* reserved2;

        jint(JNICALL* DestroyJavaVM)(JavaVM* vm);

        jint(JNICALL* AttachCurrentThread)(JavaVM* vm, void** penv, void* args);

        jint(JNICALL* DetachCurrentThread)(JavaVM* vm);

        jint(JNICALL* GetEnv)(JavaVM* vm, void** penv, jint version);

        jint(JNICALL* AttachCurrentThreadAsDaemon)(JavaVM* vm, void** penv, void* args);
    };

    struct JavaVM_ {
        const struct JNIInvokeInterface_* functions;
#ifdef __cplusplus

        jint DestroyJavaVM() {
            return functions->DestroyJavaVM(this);
        }
        jint AttachCurrentThread(void** penv, void* args) {
            return functions->AttachCurrentThread(this, penv, args);
        }
        jint DetachCurrentThread() {
            return functions->DetachCurrentThread(this);
        }

        jint GetEnv(void** penv, jint version) {
            return functions->GetEnv(this, penv, version);
        }
        jint AttachCurrentThreadAsDaemon(void** penv, void* args) {
            return functions->AttachCurrentThreadAsDaemon(this, penv, args);
        }
#endif
    };

#ifdef _JNI_IMPLEMENTATION_
#define _JNI_IMPORT_OR_EXPORT_ JNIEXPORT
#else
#define _JNI_IMPORT_OR_EXPORT_ JNIIMPORT
#endif
    _JNI_IMPORT_OR_EXPORT_ jint JNICALL
        JNI_GetDefaultJavaVMInitArgs(void* args);

    _JNI_IMPORT_OR_EXPORT_ jint JNICALL
        JNI_CreateJavaVM(JavaVM** pvm, void** penv, void* args);

    _JNI_IMPORT_OR_EXPORT_ jint JNICALL
        JNI_GetCreatedJavaVMs(JavaVM**, jsize, jsize*);

    /* Defined by native libraries. */
    JNIEXPORT jint JNICALL
        JNI_OnLoad(JavaVM* vm, void* reserved);

    JNIEXPORT void JNICALL
        JNI_OnUnload(JavaVM* vm, void* reserved);

#define JNI_VERSION_1_1 0x00010001
#define JNI_VERSION_1_2 0x00010002
#define JNI_VERSION_1_4 0x00010004
#define JNI_VERSION_1_6 0x00010006
#define JNI_VERSION_1_8 0x00010008
#define JNI_VERSION_9   0x00090000
#define JNI_VERSION_10  0x000a0000

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif // LIBNATIVEHELPER_INCLUDE_JNI_JNI_H_

#else /*For Linux*/
/*
* ========================================================================
* ========================================================================
* ========================================================================
* ========================================================================
* ========================================================================
* ========================================================================
* ========================================================================
* ========================================================================
* ========================================================================
* ========================================================================
* ========================================================================
*/

#ifndef LIBNATIVEHELPER_INCLUDE_JNI_JNI_H_
#define LIBNATIVEHELPER_INCLUDE_JNI_JNI_H_

#include <stdarg.h>
#include <stdint.h>

/* Primitive types that match up with Java equivalents. */
typedef uint8_t  jboolean; /* unsigned 8 bits */
typedef int8_t   jbyte;    /* signed 8 bits */
typedef uint16_t jchar;    /* unsigned 16 bits */
typedef int16_t  jshort;   /* signed 16 bits */
typedef int32_t  jint;     /* signed 32 bits */
typedef int64_t  jlong;    /* signed 64 bits */
typedef float    jfloat;   /* 32-bit IEEE 754 */
typedef double   jdouble;  /* 64-bit IEEE 754 */

/* "cardinal indices and sizes" */
typedef jint     jsize;

#ifdef __cplusplus
/*
 * Reference types, in C++
 */
class _jobject {};
class _jclass : public _jobject {};
class _jstring : public _jobject {};
class _jarray : public _jobject {};
class _jobjectArray : public _jarray {};
class _jbooleanArray : public _jarray {};
class _jbyteArray : public _jarray {};
class _jcharArray : public _jarray {};
class _jshortArray : public _jarray {};
class _jintArray : public _jarray {};
class _jlongArray : public _jarray {};
class _jfloatArray : public _jarray {};
class _jdoubleArray : public _jarray {};
class _jthrowable : public _jobject {};

typedef _jobject* jobject;
typedef _jclass* jclass;
typedef _jstring* jstring;
typedef _jarray* jarray;
typedef _jobjectArray* jobjectArray;
typedef _jbooleanArray* jbooleanArray;
typedef _jbyteArray* jbyteArray;
typedef _jcharArray* jcharArray;
typedef _jshortArray* jshortArray;
typedef _jintArray* jintArray;
typedef _jlongArray* jlongArray;
typedef _jfloatArray* jfloatArray;
typedef _jdoubleArray* jdoubleArray;
typedef _jthrowable* jthrowable;
typedef _jobject* jweak;


#else /* not __cplusplus */

/*
 * Reference types, in C.
 */
typedef void* jobject;
typedef jobject         jclass;
typedef jobject         jstring;
typedef jobject         jarray;
typedef jarray          jobjectArray;
typedef jarray          jbooleanArray;
typedef jarray          jbyteArray;
typedef jarray          jcharArray;
typedef jarray          jshortArray;
typedef jarray          jintArray;
typedef jarray          jlongArray;
typedef jarray          jfloatArray;
typedef jarray          jdoubleArray;
typedef jobject         jthrowable;
typedef jobject         jweak;

#endif /* not __cplusplus */

struct _jfieldID;                       /* opaque structure */
typedef struct _jfieldID* jfieldID;     /* field IDs */

struct _jmethodID;                      /* opaque structure */
typedef struct _jmethodID* jmethodID;   /* method IDs */

struct JNIInvokeInterface;

typedef union jvalue {
    jboolean    z;
    jbyte       b;
    jchar       c;
    jshort      s;
    jint        i;
    jlong       j;
    jfloat      f;
    jdouble     d;
    jobject     l;
} jvalue;

typedef enum jobjectRefType {
    JNIInvalidRefType = 0,
    JNILocalRefType = 1,
    JNIGlobalRefType = 2,
    JNIWeakGlobalRefType = 3
} jobjectRefType;

typedef struct {
    const char* name;
    const char* signature;
    void* fnPtr;
} JNINativeMethod;

struct _JNIEnv;
struct _JavaVM;
typedef const struct JNINativeInterface* C_JNIEnv;

#if defined(__cplusplus)
typedef _JNIEnv JNIEnv;
typedef _JavaVM JavaVM;
#else
typedef const struct JNINativeInterface* JNIEnv;
typedef const struct JNIInvokeInterface* JavaVM;
#endif

/*
 * Table of interface function pointers.
 */
struct JNINativeInterface {
    void* reserved0;
    void* reserved1;
    void* reserved2;
    void* reserved3;

    jint(*GetVersion)(JNIEnv*);

    jclass(*DefineClass)(JNIEnv*, const char*, jobject, const jbyte*,
        jsize);
    jclass(*FindClass)(JNIEnv*, const char*);

    jmethodID(*FromReflectedMethod)(JNIEnv*, jobject);
    jfieldID(*FromReflectedField)(JNIEnv*, jobject);
    /* spec doesn't show jboolean parameter */
    jobject(*ToReflectedMethod)(JNIEnv*, jclass, jmethodID, jboolean);

    jclass(*GetSuperclass)(JNIEnv*, jclass);
    jboolean(*IsAssignableFrom)(JNIEnv*, jclass, jclass);

    /* spec doesn't show jboolean parameter */
    jobject(*ToReflectedField)(JNIEnv*, jclass, jfieldID, jboolean);

    jint(*Throw)(JNIEnv*, jthrowable);
    jint(*ThrowNew)(JNIEnv*, jclass, const char*);
    jthrowable(*ExceptionOccurred)(JNIEnv*);
    void        (*ExceptionDescribe)(JNIEnv*);
    void        (*ExceptionClear)(JNIEnv*);
    void        (*FatalError)(JNIEnv*, const char*);

    jint(*PushLocalFrame)(JNIEnv*, jint);
    jobject(*PopLocalFrame)(JNIEnv*, jobject);

    jobject(*NewGlobalRef)(JNIEnv*, jobject);
    void        (*DeleteGlobalRef)(JNIEnv*, jobject);
    void        (*DeleteLocalRef)(JNIEnv*, jobject);
    jboolean(*IsSameObject)(JNIEnv*, jobject, jobject);

    jobject(*NewLocalRef)(JNIEnv*, jobject);
    jint(*EnsureLocalCapacity)(JNIEnv*, jint);

    jobject(*AllocObject)(JNIEnv*, jclass);
    jobject(*NewObject)(JNIEnv*, jclass, jmethodID, ...);
    jobject(*NewObjectV)(JNIEnv*, jclass, jmethodID, va_list);
    jobject(*NewObjectA)(JNIEnv*, jclass, jmethodID, const jvalue*);

    jclass(*GetObjectClass)(JNIEnv*, jobject);
    jboolean(*IsInstanceOf)(JNIEnv*, jobject, jclass);
    jmethodID(*GetMethodID)(JNIEnv*, jclass, const char*, const char*);

    jobject(*CallObjectMethod)(JNIEnv*, jobject, jmethodID, ...);
    jobject(*CallObjectMethodV)(JNIEnv*, jobject, jmethodID, va_list);
    jobject(*CallObjectMethodA)(JNIEnv*, jobject, jmethodID, const jvalue*);
    jboolean(*CallBooleanMethod)(JNIEnv*, jobject, jmethodID, ...);
    jboolean(*CallBooleanMethodV)(JNIEnv*, jobject, jmethodID, va_list);
    jboolean(*CallBooleanMethodA)(JNIEnv*, jobject, jmethodID, const jvalue*);
    jbyte(*CallByteMethod)(JNIEnv*, jobject, jmethodID, ...);
    jbyte(*CallByteMethodV)(JNIEnv*, jobject, jmethodID, va_list);
    jbyte(*CallByteMethodA)(JNIEnv*, jobject, jmethodID, const jvalue*);
    jchar(*CallCharMethod)(JNIEnv*, jobject, jmethodID, ...);
    jchar(*CallCharMethodV)(JNIEnv*, jobject, jmethodID, va_list);
    jchar(*CallCharMethodA)(JNIEnv*, jobject, jmethodID, const jvalue*);
    jshort(*CallShortMethod)(JNIEnv*, jobject, jmethodID, ...);
    jshort(*CallShortMethodV)(JNIEnv*, jobject, jmethodID, va_list);
    jshort(*CallShortMethodA)(JNIEnv*, jobject, jmethodID, const jvalue*);
    jint(*CallIntMethod)(JNIEnv*, jobject, jmethodID, ...);
    jint(*CallIntMethodV)(JNIEnv*, jobject, jmethodID, va_list);
    jint(*CallIntMethodA)(JNIEnv*, jobject, jmethodID, const jvalue*);
    jlong(*CallLongMethod)(JNIEnv*, jobject, jmethodID, ...);
    jlong(*CallLongMethodV)(JNIEnv*, jobject, jmethodID, va_list);
    jlong(*CallLongMethodA)(JNIEnv*, jobject, jmethodID, const jvalue*);
    jfloat(*CallFloatMethod)(JNIEnv*, jobject, jmethodID, ...);
    jfloat(*CallFloatMethodV)(JNIEnv*, jobject, jmethodID, va_list);
    jfloat(*CallFloatMethodA)(JNIEnv*, jobject, jmethodID, const jvalue*);
    jdouble(*CallDoubleMethod)(JNIEnv*, jobject, jmethodID, ...);
    jdouble(*CallDoubleMethodV)(JNIEnv*, jobject, jmethodID, va_list);
    jdouble(*CallDoubleMethodA)(JNIEnv*, jobject, jmethodID, const jvalue*);
    void        (*CallVoidMethod)(JNIEnv*, jobject, jmethodID, ...);
    void        (*CallVoidMethodV)(JNIEnv*, jobject, jmethodID, va_list);
    void        (*CallVoidMethodA)(JNIEnv*, jobject, jmethodID, const jvalue*);

    jobject(*CallNonvirtualObjectMethod)(JNIEnv*, jobject, jclass,
        jmethodID, ...);
    jobject(*CallNonvirtualObjectMethodV)(JNIEnv*, jobject, jclass,
        jmethodID, va_list);
    jobject(*CallNonvirtualObjectMethodA)(JNIEnv*, jobject, jclass,
        jmethodID, const jvalue*);
    jboolean(*CallNonvirtualBooleanMethod)(JNIEnv*, jobject, jclass,
        jmethodID, ...);
    jboolean(*CallNonvirtualBooleanMethodV)(JNIEnv*, jobject, jclass,
        jmethodID, va_list);
    jboolean(*CallNonvirtualBooleanMethodA)(JNIEnv*, jobject, jclass,
        jmethodID, const jvalue*);
    jbyte(*CallNonvirtualByteMethod)(JNIEnv*, jobject, jclass,
        jmethodID, ...);
    jbyte(*CallNonvirtualByteMethodV)(JNIEnv*, jobject, jclass,
        jmethodID, va_list);
    jbyte(*CallNonvirtualByteMethodA)(JNIEnv*, jobject, jclass,
        jmethodID, const jvalue*);
    jchar(*CallNonvirtualCharMethod)(JNIEnv*, jobject, jclass,
        jmethodID, ...);
    jchar(*CallNonvirtualCharMethodV)(JNIEnv*, jobject, jclass,
        jmethodID, va_list);
    jchar(*CallNonvirtualCharMethodA)(JNIEnv*, jobject, jclass,
        jmethodID, const jvalue*);
    jshort(*CallNonvirtualShortMethod)(JNIEnv*, jobject, jclass,
        jmethodID, ...);
    jshort(*CallNonvirtualShortMethodV)(JNIEnv*, jobject, jclass,
        jmethodID, va_list);
    jshort(*CallNonvirtualShortMethodA)(JNIEnv*, jobject, jclass,
        jmethodID, const jvalue*);
    jint(*CallNonvirtualIntMethod)(JNIEnv*, jobject, jclass,
        jmethodID, ...);
    jint(*CallNonvirtualIntMethodV)(JNIEnv*, jobject, jclass,
        jmethodID, va_list);
    jint(*CallNonvirtualIntMethodA)(JNIEnv*, jobject, jclass,
        jmethodID, const jvalue*);
    jlong(*CallNonvirtualLongMethod)(JNIEnv*, jobject, jclass,
        jmethodID, ...);
    jlong(*CallNonvirtualLongMethodV)(JNIEnv*, jobject, jclass,
        jmethodID, va_list);
    jlong(*CallNonvirtualLongMethodA)(JNIEnv*, jobject, jclass,
        jmethodID, const jvalue*);
    jfloat(*CallNonvirtualFloatMethod)(JNIEnv*, jobject, jclass,
        jmethodID, ...);
    jfloat(*CallNonvirtualFloatMethodV)(JNIEnv*, jobject, jclass,
        jmethodID, va_list);
    jfloat(*CallNonvirtualFloatMethodA)(JNIEnv*, jobject, jclass,
        jmethodID, const jvalue*);
    jdouble(*CallNonvirtualDoubleMethod)(JNIEnv*, jobject, jclass,
        jmethodID, ...);
    jdouble(*CallNonvirtualDoubleMethodV)(JNIEnv*, jobject, jclass,
        jmethodID, va_list);
    jdouble(*CallNonvirtualDoubleMethodA)(JNIEnv*, jobject, jclass,
        jmethodID, const jvalue*);
    void        (*CallNonvirtualVoidMethod)(JNIEnv*, jobject, jclass,
        jmethodID, ...);
    void        (*CallNonvirtualVoidMethodV)(JNIEnv*, jobject, jclass,
        jmethodID, va_list);
    void        (*CallNonvirtualVoidMethodA)(JNIEnv*, jobject, jclass,
        jmethodID, const jvalue*);

    jfieldID(*GetFieldID)(JNIEnv*, jclass, const char*, const char*);

    jobject(*GetObjectField)(JNIEnv*, jobject, jfieldID);
    jboolean(*GetBooleanField)(JNIEnv*, jobject, jfieldID);
    jbyte(*GetByteField)(JNIEnv*, jobject, jfieldID);
    jchar(*GetCharField)(JNIEnv*, jobject, jfieldID);
    jshort(*GetShortField)(JNIEnv*, jobject, jfieldID);
    jint(*GetIntField)(JNIEnv*, jobject, jfieldID);
    jlong(*GetLongField)(JNIEnv*, jobject, jfieldID);
    jfloat(*GetFloatField)(JNIEnv*, jobject, jfieldID);
    jdouble(*GetDoubleField)(JNIEnv*, jobject, jfieldID);

    void        (*SetObjectField)(JNIEnv*, jobject, jfieldID, jobject);
    void        (*SetBooleanField)(JNIEnv*, jobject, jfieldID, jboolean);
    void        (*SetByteField)(JNIEnv*, jobject, jfieldID, jbyte);
    void        (*SetCharField)(JNIEnv*, jobject, jfieldID, jchar);
    void        (*SetShortField)(JNIEnv*, jobject, jfieldID, jshort);
    void        (*SetIntField)(JNIEnv*, jobject, jfieldID, jint);
    void        (*SetLongField)(JNIEnv*, jobject, jfieldID, jlong);
    void        (*SetFloatField)(JNIEnv*, jobject, jfieldID, jfloat);
    void        (*SetDoubleField)(JNIEnv*, jobject, jfieldID, jdouble);

    jmethodID(*GetStaticMethodID)(JNIEnv*, jclass, const char*, const char*);

    jobject(*CallStaticObjectMethod)(JNIEnv*, jclass, jmethodID, ...);
    jobject(*CallStaticObjectMethodV)(JNIEnv*, jclass, jmethodID, va_list);
    jobject(*CallStaticObjectMethodA)(JNIEnv*, jclass, jmethodID, const jvalue*);
    jboolean(*CallStaticBooleanMethod)(JNIEnv*, jclass, jmethodID, ...);
    jboolean(*CallStaticBooleanMethodV)(JNIEnv*, jclass, jmethodID,
        va_list);
    jboolean(*CallStaticBooleanMethodA)(JNIEnv*, jclass, jmethodID, const jvalue*);
    jbyte(*CallStaticByteMethod)(JNIEnv*, jclass, jmethodID, ...);
    jbyte(*CallStaticByteMethodV)(JNIEnv*, jclass, jmethodID, va_list);
    jbyte(*CallStaticByteMethodA)(JNIEnv*, jclass, jmethodID, const jvalue*);
    jchar(*CallStaticCharMethod)(JNIEnv*, jclass, jmethodID, ...);
    jchar(*CallStaticCharMethodV)(JNIEnv*, jclass, jmethodID, va_list);
    jchar(*CallStaticCharMethodA)(JNIEnv*, jclass, jmethodID, const jvalue*);
    jshort(*CallStaticShortMethod)(JNIEnv*, jclass, jmethodID, ...);
    jshort(*CallStaticShortMethodV)(JNIEnv*, jclass, jmethodID, va_list);
    jshort(*CallStaticShortMethodA)(JNIEnv*, jclass, jmethodID, const jvalue*);
    jint(*CallStaticIntMethod)(JNIEnv*, jclass, jmethodID, ...);
    jint(*CallStaticIntMethodV)(JNIEnv*, jclass, jmethodID, va_list);
    jint(*CallStaticIntMethodA)(JNIEnv*, jclass, jmethodID, const jvalue*);
    jlong(*CallStaticLongMethod)(JNIEnv*, jclass, jmethodID, ...);
    jlong(*CallStaticLongMethodV)(JNIEnv*, jclass, jmethodID, va_list);
    jlong(*CallStaticLongMethodA)(JNIEnv*, jclass, jmethodID, const jvalue*);
    jfloat(*CallStaticFloatMethod)(JNIEnv*, jclass, jmethodID, ...);
    jfloat(*CallStaticFloatMethodV)(JNIEnv*, jclass, jmethodID, va_list);
    jfloat(*CallStaticFloatMethodA)(JNIEnv*, jclass, jmethodID, const jvalue*);
    jdouble(*CallStaticDoubleMethod)(JNIEnv*, jclass, jmethodID, ...);
    jdouble(*CallStaticDoubleMethodV)(JNIEnv*, jclass, jmethodID, va_list);
    jdouble(*CallStaticDoubleMethodA)(JNIEnv*, jclass, jmethodID, const jvalue*);
    void        (*CallStaticVoidMethod)(JNIEnv*, jclass, jmethodID, ...);
    void        (*CallStaticVoidMethodV)(JNIEnv*, jclass, jmethodID, va_list);
    void        (*CallStaticVoidMethodA)(JNIEnv*, jclass, jmethodID, const jvalue*);

    jfieldID(*GetStaticFieldID)(JNIEnv*, jclass, const char*,
        const char*);

    jobject(*GetStaticObjectField)(JNIEnv*, jclass, jfieldID);
    jboolean(*GetStaticBooleanField)(JNIEnv*, jclass, jfieldID);
    jbyte(*GetStaticByteField)(JNIEnv*, jclass, jfieldID);
    jchar(*GetStaticCharField)(JNIEnv*, jclass, jfieldID);
    jshort(*GetStaticShortField)(JNIEnv*, jclass, jfieldID);
    jint(*GetStaticIntField)(JNIEnv*, jclass, jfieldID);
    jlong(*GetStaticLongField)(JNIEnv*, jclass, jfieldID);
    jfloat(*GetStaticFloatField)(JNIEnv*, jclass, jfieldID);
    jdouble(*GetStaticDoubleField)(JNIEnv*, jclass, jfieldID);

    void        (*SetStaticObjectField)(JNIEnv*, jclass, jfieldID, jobject);
    void        (*SetStaticBooleanField)(JNIEnv*, jclass, jfieldID, jboolean);
    void        (*SetStaticByteField)(JNIEnv*, jclass, jfieldID, jbyte);
    void        (*SetStaticCharField)(JNIEnv*, jclass, jfieldID, jchar);
    void        (*SetStaticShortField)(JNIEnv*, jclass, jfieldID, jshort);
    void        (*SetStaticIntField)(JNIEnv*, jclass, jfieldID, jint);
    void        (*SetStaticLongField)(JNIEnv*, jclass, jfieldID, jlong);
    void        (*SetStaticFloatField)(JNIEnv*, jclass, jfieldID, jfloat);
    void        (*SetStaticDoubleField)(JNIEnv*, jclass, jfieldID, jdouble);

    jstring(*NewString)(JNIEnv*, const jchar*, jsize);
    jsize(*GetStringLength)(JNIEnv*, jstring);
    const jchar* (*GetStringChars)(JNIEnv*, jstring, jboolean*);
    void        (*ReleaseStringChars)(JNIEnv*, jstring, const jchar*);
    jstring(*NewStringUTF)(JNIEnv*, const char*);
    jsize(*GetStringUTFLength)(JNIEnv*, jstring);
    /* JNI spec says this returns const jbyte*, but that's inconsistent */
    const char* (*GetStringUTFChars)(JNIEnv*, jstring, jboolean*);
    void        (*ReleaseStringUTFChars)(JNIEnv*, jstring, const char*);
    jsize(*GetArrayLength)(JNIEnv*, jarray);
    jobjectArray(*NewObjectArray)(JNIEnv*, jsize, jclass, jobject);
    jobject(*GetObjectArrayElement)(JNIEnv*, jobjectArray, jsize);
    void        (*SetObjectArrayElement)(JNIEnv*, jobjectArray, jsize, jobject);

    jbooleanArray(*NewBooleanArray)(JNIEnv*, jsize);
    jbyteArray(*NewByteArray)(JNIEnv*, jsize);
    jcharArray(*NewCharArray)(JNIEnv*, jsize);
    jshortArray(*NewShortArray)(JNIEnv*, jsize);
    jintArray(*NewIntArray)(JNIEnv*, jsize);
    jlongArray(*NewLongArray)(JNIEnv*, jsize);
    jfloatArray(*NewFloatArray)(JNIEnv*, jsize);
    jdoubleArray(*NewDoubleArray)(JNIEnv*, jsize);

    jboolean* (*GetBooleanArrayElements)(JNIEnv*, jbooleanArray, jboolean*);
    jbyte* (*GetByteArrayElements)(JNIEnv*, jbyteArray, jboolean*);
    jchar* (*GetCharArrayElements)(JNIEnv*, jcharArray, jboolean*);
    jshort* (*GetShortArrayElements)(JNIEnv*, jshortArray, jboolean*);
    jint* (*GetIntArrayElements)(JNIEnv*, jintArray, jboolean*);
    jlong* (*GetLongArrayElements)(JNIEnv*, jlongArray, jboolean*);
    jfloat* (*GetFloatArrayElements)(JNIEnv*, jfloatArray, jboolean*);
    jdouble* (*GetDoubleArrayElements)(JNIEnv*, jdoubleArray, jboolean*);

    void        (*ReleaseBooleanArrayElements)(JNIEnv*, jbooleanArray,
        jboolean*, jint);
    void        (*ReleaseByteArrayElements)(JNIEnv*, jbyteArray,
        jbyte*, jint);
    void        (*ReleaseCharArrayElements)(JNIEnv*, jcharArray,
        jchar*, jint);
    void        (*ReleaseShortArrayElements)(JNIEnv*, jshortArray,
        jshort*, jint);
    void        (*ReleaseIntArrayElements)(JNIEnv*, jintArray,
        jint*, jint);
    void        (*ReleaseLongArrayElements)(JNIEnv*, jlongArray,
        jlong*, jint);
    void        (*ReleaseFloatArrayElements)(JNIEnv*, jfloatArray,
        jfloat*, jint);
    void        (*ReleaseDoubleArrayElements)(JNIEnv*, jdoubleArray,
        jdouble*, jint);

    void        (*GetBooleanArrayRegion)(JNIEnv*, jbooleanArray,
        jsize, jsize, jboolean*);
    void        (*GetByteArrayRegion)(JNIEnv*, jbyteArray,
        jsize, jsize, jbyte*);
    void        (*GetCharArrayRegion)(JNIEnv*, jcharArray,
        jsize, jsize, jchar*);
    void        (*GetShortArrayRegion)(JNIEnv*, jshortArray,
        jsize, jsize, jshort*);
    void        (*GetIntArrayRegion)(JNIEnv*, jintArray,
        jsize, jsize, jint*);
    void        (*GetLongArrayRegion)(JNIEnv*, jlongArray,
        jsize, jsize, jlong*);
    void        (*GetFloatArrayRegion)(JNIEnv*, jfloatArray,
        jsize, jsize, jfloat*);
    void        (*GetDoubleArrayRegion)(JNIEnv*, jdoubleArray,
        jsize, jsize, jdouble*);

    /* spec shows these without const; some jni.h do, some don't */
    void        (*SetBooleanArrayRegion)(JNIEnv*, jbooleanArray,
        jsize, jsize, const jboolean*);
    void        (*SetByteArrayRegion)(JNIEnv*, jbyteArray,
        jsize, jsize, const jbyte*);
    void        (*SetCharArrayRegion)(JNIEnv*, jcharArray,
        jsize, jsize, const jchar*);
    void        (*SetShortArrayRegion)(JNIEnv*, jshortArray,
        jsize, jsize, const jshort*);
    void        (*SetIntArrayRegion)(JNIEnv*, jintArray,
        jsize, jsize, const jint*);
    void        (*SetLongArrayRegion)(JNIEnv*, jlongArray,
        jsize, jsize, const jlong*);
    void        (*SetFloatArrayRegion)(JNIEnv*, jfloatArray,
        jsize, jsize, const jfloat*);
    void        (*SetDoubleArrayRegion)(JNIEnv*, jdoubleArray,
        jsize, jsize, const jdouble*);

    jint(*RegisterNatives)(JNIEnv*, jclass, const JNINativeMethod*,
        jint);
    jint(*UnregisterNatives)(JNIEnv*, jclass);
    jint(*MonitorEnter)(JNIEnv*, jobject);
    jint(*MonitorExit)(JNIEnv*, jobject);
    jint(*GetJavaVM)(JNIEnv*, JavaVM**);

    void        (*GetStringRegion)(JNIEnv*, jstring, jsize, jsize, jchar*);
    void        (*GetStringUTFRegion)(JNIEnv*, jstring, jsize, jsize, char*);

    void* (*GetPrimitiveArrayCritical)(JNIEnv*, jarray, jboolean*);
    void        (*ReleasePrimitiveArrayCritical)(JNIEnv*, jarray, void*, jint);

    const jchar* (*GetStringCritical)(JNIEnv*, jstring, jboolean*);
    void        (*ReleaseStringCritical)(JNIEnv*, jstring, const jchar*);

    jweak(*NewWeakGlobalRef)(JNIEnv*, jobject);
    void        (*DeleteWeakGlobalRef)(JNIEnv*, jweak);

    jboolean(*ExceptionCheck)(JNIEnv*);

    jobject(*NewDirectByteBuffer)(JNIEnv*, void*, jlong);
    void* (*GetDirectBufferAddress)(JNIEnv*, jobject);
    jlong(*GetDirectBufferCapacity)(JNIEnv*, jobject);

    /* added in JNI 1.6 */
    jobjectRefType(*GetObjectRefType)(JNIEnv*, jobject);
};

/*
 * C++ object wrapper.
 *
 * This is usually overlaid on a C struct whose first element is a
 * JNINativeInterface*.  We rely somewhat on compiler behavior.
 */
struct _JNIEnv {
    /* do not rename this; it does not seem to be entirely opaque */
    const struct JNINativeInterface* functions;

#if defined(__cplusplus)

    jint GetVersion()
    {
        return functions->GetVersion(this);
    }

    jclass DefineClass(const char* name, jobject loader, const jbyte* buf,
        jsize bufLen)
    {
        return functions->DefineClass(this, name, loader, buf, bufLen);
    }

    jclass FindClass(const char* name)
    {
        return functions->FindClass(this, name);
    }

    jmethodID FromReflectedMethod(jobject method)
    {
        return functions->FromReflectedMethod(this, method);
    }

    jfieldID FromReflectedField(jobject field)
    {
        return functions->FromReflectedField(this, field);
    }

    jobject ToReflectedMethod(jclass cls, jmethodID methodID, jboolean isStatic)
    {
        return functions->ToReflectedMethod(this, cls, methodID, isStatic);
    }

    jclass GetSuperclass(jclass clazz)
    {
        return functions->GetSuperclass(this, clazz);
    }

    jboolean IsAssignableFrom(jclass clazz1, jclass clazz2)
    {
        return functions->IsAssignableFrom(this, clazz1, clazz2);
    }

    jobject ToReflectedField(jclass cls, jfieldID fieldID, jboolean isStatic)
    {
        return functions->ToReflectedField(this, cls, fieldID, isStatic);
    }

    jint Throw(jthrowable obj)
    {
        return functions->Throw(this, obj);
    }

    jint ThrowNew(jclass clazz, const char* message)
    {
        return functions->ThrowNew(this, clazz, message);
    }

    jthrowable ExceptionOccurred()
    {
        return functions->ExceptionOccurred(this);
    }

    void ExceptionDescribe()
    {
        functions->ExceptionDescribe(this);
    }

    void ExceptionClear()
    {
        functions->ExceptionClear(this);
    }

    void FatalError(const char* msg)
    {
        functions->FatalError(this, msg);
    }

    jint PushLocalFrame(jint capacity)
    {
        return functions->PushLocalFrame(this, capacity);
    }

    jobject PopLocalFrame(jobject result)
    {
        return functions->PopLocalFrame(this, result);
    }

    jobject NewGlobalRef(jobject obj)
    {
        return functions->NewGlobalRef(this, obj);
    }

    void DeleteGlobalRef(jobject globalRef)
    {
        functions->DeleteGlobalRef(this, globalRef);
    }

    void DeleteLocalRef(jobject localRef)
    {
        functions->DeleteLocalRef(this, localRef);
    }

    jboolean IsSameObject(jobject ref1, jobject ref2)
    {
        return functions->IsSameObject(this, ref1, ref2);
    }

    jobject NewLocalRef(jobject ref)
    {
        return functions->NewLocalRef(this, ref);
    }

    jint EnsureLocalCapacity(jint capacity)
    {
        return functions->EnsureLocalCapacity(this, capacity);
    }

    jobject AllocObject(jclass clazz)
    {
        return functions->AllocObject(this, clazz);
    }

    jobject NewObject(jclass clazz, jmethodID methodID, ...)
    {
        va_list args;
        va_start(args, methodID);
        jobject result = functions->NewObjectV(this, clazz, methodID, args);
        va_end(args);
        return result;
    }

    jobject NewObjectV(jclass clazz, jmethodID methodID, va_list args)
    {
        return functions->NewObjectV(this, clazz, methodID, args);
    }

    jobject NewObjectA(jclass clazz, jmethodID methodID, const jvalue* args)
    {
        return functions->NewObjectA(this, clazz, methodID, args);
    }

    jclass GetObjectClass(jobject obj)
    {
        return functions->GetObjectClass(this, obj);
    }

    jboolean IsInstanceOf(jobject obj, jclass clazz)
    {
        return functions->IsInstanceOf(this, obj, clazz);
    }

    jmethodID GetMethodID(jclass clazz, const char* name, const char* sig)
    {
        return functions->GetMethodID(this, clazz, name, sig);
    }

#define CALL_TYPE_METHOD(_jtype, _jname)                                    \
    _jtype Call##_jname##Method(jobject obj, jmethodID methodID, ...)       \
    {                                                                       \
        _jtype result;                                                      \
        va_list args;                                                       \
        va_start(args, methodID);                                           \
        result = functions->Call##_jname##MethodV(this, obj, methodID,      \
                    args);                                                  \
        va_end(args);                                                       \
        return result;                                                      \
    }
#define CALL_TYPE_METHODV(_jtype, _jname)                                   \
    _jtype Call##_jname##MethodV(jobject obj, jmethodID methodID,           \
        va_list args)                                                       \
    { return functions->Call##_jname##MethodV(this, obj, methodID, args); }
#define CALL_TYPE_METHODA(_jtype, _jname)                                   \
    _jtype Call##_jname##MethodA(jobject obj, jmethodID methodID,           \
                                 const jvalue* args)                        \
    { return functions->Call##_jname##MethodA(this, obj, methodID, args); }

#define CALL_TYPE(_jtype, _jname)                                           \
    CALL_TYPE_METHOD(_jtype, _jname)                                        \
    CALL_TYPE_METHODV(_jtype, _jname)                                       \
    CALL_TYPE_METHODA(_jtype, _jname)

    CALL_TYPE(jobject, Object)
        CALL_TYPE(jboolean, Boolean)
        CALL_TYPE(jbyte, Byte)
        CALL_TYPE(jchar, Char)
        CALL_TYPE(jshort, Short)
        CALL_TYPE(jint, Int)
        CALL_TYPE(jlong, Long)
        CALL_TYPE(jfloat, Float)
        CALL_TYPE(jdouble, Double)

        void CallVoidMethod(jobject obj, jmethodID methodID, ...)
    {
        va_list args;
        va_start(args, methodID);
        functions->CallVoidMethodV(this, obj, methodID, args);
        va_end(args);
    }
    void CallVoidMethodV(jobject obj, jmethodID methodID, va_list args)
    {
        functions->CallVoidMethodV(this, obj, methodID, args);
    }
    void CallVoidMethodA(jobject obj, jmethodID methodID, const jvalue* args)
    {
        functions->CallVoidMethodA(this, obj, methodID, args);
    }

#define CALL_NONVIRT_TYPE_METHOD(_jtype, _jname)                            \
    _jtype CallNonvirtual##_jname##Method(jobject obj, jclass clazz,        \
        jmethodID methodID, ...)                                            \
    {                                                                       \
        _jtype result;                                                      \
        va_list args;                                                       \
        va_start(args, methodID);                                           \
        result = functions->CallNonvirtual##_jname##MethodV(this, obj,      \
                    clazz, methodID, args);                                 \
        va_end(args);                                                       \
        return result;                                                      \
    }
#define CALL_NONVIRT_TYPE_METHODV(_jtype, _jname)                           \
    _jtype CallNonvirtual##_jname##MethodV(jobject obj, jclass clazz,       \
        jmethodID methodID, va_list args)                                   \
    { return functions->CallNonvirtual##_jname##MethodV(this, obj, clazz,   \
        methodID, args); }
#define CALL_NONVIRT_TYPE_METHODA(_jtype, _jname)                           \
    _jtype CallNonvirtual##_jname##MethodA(jobject obj, jclass clazz,       \
        jmethodID methodID, const jvalue* args)                             \
    { return functions->CallNonvirtual##_jname##MethodA(this, obj, clazz,   \
        methodID, args); }

#define CALL_NONVIRT_TYPE(_jtype, _jname)                                   \
    CALL_NONVIRT_TYPE_METHOD(_jtype, _jname)                                \
    CALL_NONVIRT_TYPE_METHODV(_jtype, _jname)                               \
    CALL_NONVIRT_TYPE_METHODA(_jtype, _jname)

    CALL_NONVIRT_TYPE(jobject, Object)
        CALL_NONVIRT_TYPE(jboolean, Boolean)
        CALL_NONVIRT_TYPE(jbyte, Byte)
        CALL_NONVIRT_TYPE(jchar, Char)
        CALL_NONVIRT_TYPE(jshort, Short)
        CALL_NONVIRT_TYPE(jint, Int)
        CALL_NONVIRT_TYPE(jlong, Long)
        CALL_NONVIRT_TYPE(jfloat, Float)
        CALL_NONVIRT_TYPE(jdouble, Double)

        void CallNonvirtualVoidMethod(jobject obj, jclass clazz,
            jmethodID methodID, ...)
    {
        va_list args;
        va_start(args, methodID);
        functions->CallNonvirtualVoidMethodV(this, obj, clazz, methodID, args);
        va_end(args);
    }
    void CallNonvirtualVoidMethodV(jobject obj, jclass clazz,
        jmethodID methodID, va_list args)
    {
        functions->CallNonvirtualVoidMethodV(this, obj, clazz, methodID, args);
    }
    void CallNonvirtualVoidMethodA(jobject obj, jclass clazz,
        jmethodID methodID, const jvalue* args)
    {
        functions->CallNonvirtualVoidMethodA(this, obj, clazz, methodID, args);
    }

    jfieldID GetFieldID(jclass clazz, const char* name, const char* sig)
    {
        return functions->GetFieldID(this, clazz, name, sig);
    }

    jobject GetObjectField(jobject obj, jfieldID fieldID)
    {
        return functions->GetObjectField(this, obj, fieldID);
    }
    jboolean GetBooleanField(jobject obj, jfieldID fieldID)
    {
        return functions->GetBooleanField(this, obj, fieldID);
    }
    jbyte GetByteField(jobject obj, jfieldID fieldID)
    {
        return functions->GetByteField(this, obj, fieldID);
    }
    jchar GetCharField(jobject obj, jfieldID fieldID)
    {
        return functions->GetCharField(this, obj, fieldID);
    }
    jshort GetShortField(jobject obj, jfieldID fieldID)
    {
        return functions->GetShortField(this, obj, fieldID);
    }
    jint GetIntField(jobject obj, jfieldID fieldID)
    {
        return functions->GetIntField(this, obj, fieldID);
    }
    jlong GetLongField(jobject obj, jfieldID fieldID)
    {
        return functions->GetLongField(this, obj, fieldID);
    }
    jfloat GetFloatField(jobject obj, jfieldID fieldID)
    {
        return functions->GetFloatField(this, obj, fieldID);
    }
    jdouble GetDoubleField(jobject obj, jfieldID fieldID)
    {
        return functions->GetDoubleField(this, obj, fieldID);
    }

    void SetObjectField(jobject obj, jfieldID fieldID, jobject value)
    {
        functions->SetObjectField(this, obj, fieldID, value);
    }
    void SetBooleanField(jobject obj, jfieldID fieldID, jboolean value)
    {
        functions->SetBooleanField(this, obj, fieldID, value);
    }
    void SetByteField(jobject obj, jfieldID fieldID, jbyte value)
    {
        functions->SetByteField(this, obj, fieldID, value);
    }
    void SetCharField(jobject obj, jfieldID fieldID, jchar value)
    {
        functions->SetCharField(this, obj, fieldID, value);
    }
    void SetShortField(jobject obj, jfieldID fieldID, jshort value)
    {
        functions->SetShortField(this, obj, fieldID, value);
    }
    void SetIntField(jobject obj, jfieldID fieldID, jint value)
    {
        functions->SetIntField(this, obj, fieldID, value);
    }
    void SetLongField(jobject obj, jfieldID fieldID, jlong value)
    {
        functions->SetLongField(this, obj, fieldID, value);
    }
    void SetFloatField(jobject obj, jfieldID fieldID, jfloat value)
    {
        functions->SetFloatField(this, obj, fieldID, value);
    }
    void SetDoubleField(jobject obj, jfieldID fieldID, jdouble value)
    {
        functions->SetDoubleField(this, obj, fieldID, value);
    }

    jmethodID GetStaticMethodID(jclass clazz, const char* name, const char* sig)
    {
        return functions->GetStaticMethodID(this, clazz, name, sig);
    }

#define CALL_STATIC_TYPE_METHOD(_jtype, _jname)                             \
    _jtype CallStatic##_jname##Method(jclass clazz, jmethodID methodID,     \
        ...)                                                                \
    {                                                                       \
        _jtype result;                                                      \
        va_list args;                                                       \
        va_start(args, methodID);                                           \
        result = functions->CallStatic##_jname##MethodV(this, clazz,        \
                    methodID, args);                                        \
        va_end(args);                                                       \
        return result;                                                      \
    }
#define CALL_STATIC_TYPE_METHODV(_jtype, _jname)                            \
    _jtype CallStatic##_jname##MethodV(jclass clazz, jmethodID methodID,    \
        va_list args)                                                       \
    { return functions->CallStatic##_jname##MethodV(this, clazz, methodID,  \
        args); }
#define CALL_STATIC_TYPE_METHODA(_jtype, _jname)                            \
    _jtype CallStatic##_jname##MethodA(jclass clazz, jmethodID methodID,    \
                                       const jvalue* args)                  \
    { return functions->CallStatic##_jname##MethodA(this, clazz, methodID,  \
        args); }

#define CALL_STATIC_TYPE(_jtype, _jname)                                    \
    CALL_STATIC_TYPE_METHOD(_jtype, _jname)                                 \
    CALL_STATIC_TYPE_METHODV(_jtype, _jname)                                \
    CALL_STATIC_TYPE_METHODA(_jtype, _jname)

    CALL_STATIC_TYPE(jobject, Object)
        CALL_STATIC_TYPE(jboolean, Boolean)
        CALL_STATIC_TYPE(jbyte, Byte)
        CALL_STATIC_TYPE(jchar, Char)
        CALL_STATIC_TYPE(jshort, Short)
        CALL_STATIC_TYPE(jint, Int)
        CALL_STATIC_TYPE(jlong, Long)
        CALL_STATIC_TYPE(jfloat, Float)
        CALL_STATIC_TYPE(jdouble, Double)

        void CallStaticVoidMethod(jclass clazz, jmethodID methodID, ...)
    {
        va_list args;
        va_start(args, methodID);
        functions->CallStaticVoidMethodV(this, clazz, methodID, args);
        va_end(args);
    }
    void CallStaticVoidMethodV(jclass clazz, jmethodID methodID, va_list args)
    {
        functions->CallStaticVoidMethodV(this, clazz, methodID, args);
    }
    void CallStaticVoidMethodA(jclass clazz, jmethodID methodID, const jvalue* args)
    {
        functions->CallStaticVoidMethodA(this, clazz, methodID, args);
    }

    jfieldID GetStaticFieldID(jclass clazz, const char* name, const char* sig)
    {
        return functions->GetStaticFieldID(this, clazz, name, sig);
    }

    jobject GetStaticObjectField(jclass clazz, jfieldID fieldID)
    {
        return functions->GetStaticObjectField(this, clazz, fieldID);
    }
    jboolean GetStaticBooleanField(jclass clazz, jfieldID fieldID)
    {
        return functions->GetStaticBooleanField(this, clazz, fieldID);
    }
    jbyte GetStaticByteField(jclass clazz, jfieldID fieldID)
    {
        return functions->GetStaticByteField(this, clazz, fieldID);
    }
    jchar GetStaticCharField(jclass clazz, jfieldID fieldID)
    {
        return functions->GetStaticCharField(this, clazz, fieldID);
    }
    jshort GetStaticShortField(jclass clazz, jfieldID fieldID)
    {
        return functions->GetStaticShortField(this, clazz, fieldID);
    }
    jint GetStaticIntField(jclass clazz, jfieldID fieldID)
    {
        return functions->GetStaticIntField(this, clazz, fieldID);
    }
    jlong GetStaticLongField(jclass clazz, jfieldID fieldID)
    {
        return functions->GetStaticLongField(this, clazz, fieldID);
    }
    jfloat GetStaticFloatField(jclass clazz, jfieldID fieldID)
    {
        return functions->GetStaticFloatField(this, clazz, fieldID);
    }
    jdouble GetStaticDoubleField(jclass clazz, jfieldID fieldID)
    {
        return functions->GetStaticDoubleField(this, clazz, fieldID);
    }

    void SetStaticObjectField(jclass clazz, jfieldID fieldID, jobject value)
    {
        functions->SetStaticObjectField(this, clazz, fieldID, value);
    }
    void SetStaticBooleanField(jclass clazz, jfieldID fieldID, jboolean value)
    {
        functions->SetStaticBooleanField(this, clazz, fieldID, value);
    }
    void SetStaticByteField(jclass clazz, jfieldID fieldID, jbyte value)
    {
        functions->SetStaticByteField(this, clazz, fieldID, value);
    }
    void SetStaticCharField(jclass clazz, jfieldID fieldID, jchar value)
    {
        functions->SetStaticCharField(this, clazz, fieldID, value);
    }
    void SetStaticShortField(jclass clazz, jfieldID fieldID, jshort value)
    {
        functions->SetStaticShortField(this, clazz, fieldID, value);
    }
    void SetStaticIntField(jclass clazz, jfieldID fieldID, jint value)
    {
        functions->SetStaticIntField(this, clazz, fieldID, value);
    }
    void SetStaticLongField(jclass clazz, jfieldID fieldID, jlong value)
    {
        functions->SetStaticLongField(this, clazz, fieldID, value);
    }
    void SetStaticFloatField(jclass clazz, jfieldID fieldID, jfloat value)
    {
        functions->SetStaticFloatField(this, clazz, fieldID, value);
    }
    void SetStaticDoubleField(jclass clazz, jfieldID fieldID, jdouble value)
    {
        functions->SetStaticDoubleField(this, clazz, fieldID, value);
    }

    jstring NewString(const jchar* unicodeChars, jsize len)
    {
        return functions->NewString(this, unicodeChars, len);
    }

    jsize GetStringLength(jstring string)
    {
        return functions->GetStringLength(this, string);
    }

    const jchar* GetStringChars(jstring string, jboolean* isCopy)
    {
        return functions->GetStringChars(this, string, isCopy);
    }

    void ReleaseStringChars(jstring string, const jchar* chars)
    {
        functions->ReleaseStringChars(this, string, chars);
    }

    jstring NewStringUTF(const char* bytes)
    {
        return functions->NewStringUTF(this, bytes);
    }

    jsize GetStringUTFLength(jstring string)
    {
        return functions->GetStringUTFLength(this, string);
    }

    const char* GetStringUTFChars(jstring string, jboolean* isCopy)
    {
        return functions->GetStringUTFChars(this, string, isCopy);
    }

    void ReleaseStringUTFChars(jstring string, const char* utf)
    {
        functions->ReleaseStringUTFChars(this, string, utf);
    }

    jsize GetArrayLength(jarray array)
    {
        return functions->GetArrayLength(this, array);
    }

    jobjectArray NewObjectArray(jsize length, jclass elementClass,
        jobject initialElement)
    {
        return functions->NewObjectArray(this, length, elementClass,
            initialElement);
    }

    jobject GetObjectArrayElement(jobjectArray array, jsize index)
    {
        return functions->GetObjectArrayElement(this, array, index);
    }

    void SetObjectArrayElement(jobjectArray array, jsize index, jobject value)
    {
        functions->SetObjectArrayElement(this, array, index, value);
    }

    jbooleanArray NewBooleanArray(jsize length)
    {
        return functions->NewBooleanArray(this, length);
    }
    jbyteArray NewByteArray(jsize length)
    {
        return functions->NewByteArray(this, length);
    }
    jcharArray NewCharArray(jsize length)
    {
        return functions->NewCharArray(this, length);
    }
    jshortArray NewShortArray(jsize length)
    {
        return functions->NewShortArray(this, length);
    }
    jintArray NewIntArray(jsize length)
    {
        return functions->NewIntArray(this, length);
    }
    jlongArray NewLongArray(jsize length)
    {
        return functions->NewLongArray(this, length);
    }
    jfloatArray NewFloatArray(jsize length)
    {
        return functions->NewFloatArray(this, length);
    }
    jdoubleArray NewDoubleArray(jsize length)
    {
        return functions->NewDoubleArray(this, length);
    }

    jboolean* GetBooleanArrayElements(jbooleanArray array, jboolean* isCopy)
    {
        return functions->GetBooleanArrayElements(this, array, isCopy);
    }
    jbyte* GetByteArrayElements(jbyteArray array, jboolean* isCopy)
    {
        return functions->GetByteArrayElements(this, array, isCopy);
    }
    jchar* GetCharArrayElements(jcharArray array, jboolean* isCopy)
    {
        return functions->GetCharArrayElements(this, array, isCopy);
    }
    jshort* GetShortArrayElements(jshortArray array, jboolean* isCopy)
    {
        return functions->GetShortArrayElements(this, array, isCopy);
    }
    jint* GetIntArrayElements(jintArray array, jboolean* isCopy)
    {
        return functions->GetIntArrayElements(this, array, isCopy);
    }
    jlong* GetLongArrayElements(jlongArray array, jboolean* isCopy)
    {
        return functions->GetLongArrayElements(this, array, isCopy);
    }
    jfloat* GetFloatArrayElements(jfloatArray array, jboolean* isCopy)
    {
        return functions->GetFloatArrayElements(this, array, isCopy);
    }
    jdouble* GetDoubleArrayElements(jdoubleArray array, jboolean* isCopy)
    {
        return functions->GetDoubleArrayElements(this, array, isCopy);
    }

    void ReleaseBooleanArrayElements(jbooleanArray array, jboolean* elems,
        jint mode)
    {
        functions->ReleaseBooleanArrayElements(this, array, elems, mode);
    }
    void ReleaseByteArrayElements(jbyteArray array, jbyte* elems,
        jint mode)
    {
        functions->ReleaseByteArrayElements(this, array, elems, mode);
    }
    void ReleaseCharArrayElements(jcharArray array, jchar* elems,
        jint mode)
    {
        functions->ReleaseCharArrayElements(this, array, elems, mode);
    }
    void ReleaseShortArrayElements(jshortArray array, jshort* elems,
        jint mode)
    {
        functions->ReleaseShortArrayElements(this, array, elems, mode);
    }
    void ReleaseIntArrayElements(jintArray array, jint* elems,
        jint mode)
    {
        functions->ReleaseIntArrayElements(this, array, elems, mode);
    }
    void ReleaseLongArrayElements(jlongArray array, jlong* elems,
        jint mode)
    {
        functions->ReleaseLongArrayElements(this, array, elems, mode);
    }
    void ReleaseFloatArrayElements(jfloatArray array, jfloat* elems,
        jint mode)
    {
        functions->ReleaseFloatArrayElements(this, array, elems, mode);
    }
    void ReleaseDoubleArrayElements(jdoubleArray array, jdouble* elems,
        jint mode)
    {
        functions->ReleaseDoubleArrayElements(this, array, elems, mode);
    }

    void GetBooleanArrayRegion(jbooleanArray array, jsize start, jsize len,
        jboolean* buf)
    {
        functions->GetBooleanArrayRegion(this, array, start, len, buf);
    }
    void GetByteArrayRegion(jbyteArray array, jsize start, jsize len,
        jbyte* buf)
    {
        functions->GetByteArrayRegion(this, array, start, len, buf);
    }
    void GetCharArrayRegion(jcharArray array, jsize start, jsize len,
        jchar* buf)
    {
        functions->GetCharArrayRegion(this, array, start, len, buf);
    }
    void GetShortArrayRegion(jshortArray array, jsize start, jsize len,
        jshort* buf)
    {
        functions->GetShortArrayRegion(this, array, start, len, buf);
    }
    void GetIntArrayRegion(jintArray array, jsize start, jsize len,
        jint* buf)
    {
        functions->GetIntArrayRegion(this, array, start, len, buf);
    }
    void GetLongArrayRegion(jlongArray array, jsize start, jsize len,
        jlong* buf)
    {
        functions->GetLongArrayRegion(this, array, start, len, buf);
    }
    void GetFloatArrayRegion(jfloatArray array, jsize start, jsize len,
        jfloat* buf)
    {
        functions->GetFloatArrayRegion(this, array, start, len, buf);
    }
    void GetDoubleArrayRegion(jdoubleArray array, jsize start, jsize len,
        jdouble* buf)
    {
        functions->GetDoubleArrayRegion(this, array, start, len, buf);
    }

    void SetBooleanArrayRegion(jbooleanArray array, jsize start, jsize len,
        const jboolean* buf)
    {
        functions->SetBooleanArrayRegion(this, array, start, len, buf);
    }
    void SetByteArrayRegion(jbyteArray array, jsize start, jsize len,
        const jbyte* buf)
    {
        functions->SetByteArrayRegion(this, array, start, len, buf);
    }
    void SetCharArrayRegion(jcharArray array, jsize start, jsize len,
        const jchar* buf)
    {
        functions->SetCharArrayRegion(this, array, start, len, buf);
    }
    void SetShortArrayRegion(jshortArray array, jsize start, jsize len,
        const jshort* buf)
    {
        functions->SetShortArrayRegion(this, array, start, len, buf);
    }
    void SetIntArrayRegion(jintArray array, jsize start, jsize len,
        const jint* buf)
    {
        functions->SetIntArrayRegion(this, array, start, len, buf);
    }
    void SetLongArrayRegion(jlongArray array, jsize start, jsize len,
        const jlong* buf)
    {
        functions->SetLongArrayRegion(this, array, start, len, buf);
    }
    void SetFloatArrayRegion(jfloatArray array, jsize start, jsize len,
        const jfloat* buf)
    {
        functions->SetFloatArrayRegion(this, array, start, len, buf);
    }
    void SetDoubleArrayRegion(jdoubleArray array, jsize start, jsize len,
        const jdouble* buf)
    {
        functions->SetDoubleArrayRegion(this, array, start, len, buf);
    }

    jint RegisterNatives(jclass clazz, const JNINativeMethod* methods,
        jint nMethods)
    {
        return functions->RegisterNatives(this, clazz, methods, nMethods);
    }

    jint UnregisterNatives(jclass clazz)
    {
        return functions->UnregisterNatives(this, clazz);
    }

    jint MonitorEnter(jobject obj)
    {
        return functions->MonitorEnter(this, obj);
    }

    jint MonitorExit(jobject obj)
    {
        return functions->MonitorExit(this, obj);
    }

    jint GetJavaVM(JavaVM** vm)
    {
        return functions->GetJavaVM(this, vm);
    }

    void GetStringRegion(jstring str, jsize start, jsize len, jchar* buf)
    {
        functions->GetStringRegion(this, str, start, len, buf);
    }

    void GetStringUTFRegion(jstring str, jsize start, jsize len, char* buf)
    {
        return functions->GetStringUTFRegion(this, str, start, len, buf);
    }

    void* GetPrimitiveArrayCritical(jarray array, jboolean* isCopy)
    {
        return functions->GetPrimitiveArrayCritical(this, array, isCopy);
    }

    void ReleasePrimitiveArrayCritical(jarray array, void* carray, jint mode)
    {
        functions->ReleasePrimitiveArrayCritical(this, array, carray, mode);
    }

    const jchar* GetStringCritical(jstring string, jboolean* isCopy)
    {
        return functions->GetStringCritical(this, string, isCopy);
    }

    void ReleaseStringCritical(jstring string, const jchar* carray)
    {
        functions->ReleaseStringCritical(this, string, carray);
    }

    jweak NewWeakGlobalRef(jobject obj)
    {
        return functions->NewWeakGlobalRef(this, obj);
    }

    void DeleteWeakGlobalRef(jweak obj)
    {
        functions->DeleteWeakGlobalRef(this, obj);
    }

    jboolean ExceptionCheck()
    {
        return functions->ExceptionCheck(this);
    }

    jobject NewDirectByteBuffer(void* address, jlong capacity)
    {
        return functions->NewDirectByteBuffer(this, address, capacity);
    }

    void* GetDirectBufferAddress(jobject buf)
    {
        return functions->GetDirectBufferAddress(this, buf);
    }

    jlong GetDirectBufferCapacity(jobject buf)
    {
        return functions->GetDirectBufferCapacity(this, buf);
    }

    /* added in JNI 1.6 */
    jobjectRefType GetObjectRefType(jobject obj)
    {
        return functions->GetObjectRefType(this, obj);
    }
#endif /*__cplusplus*/
};


/*
 * JNI invocation interface.
 */
struct JNIInvokeInterface {
    void* reserved0;
    void* reserved1;
    void* reserved2;

    jint(*DestroyJavaVM)(JavaVM*);
    jint(*AttachCurrentThread)(JavaVM*, JNIEnv**, void*);
    jint(*DetachCurrentThread)(JavaVM*);
    jint(*GetEnv)(JavaVM*, void**, jint);
    jint(*AttachCurrentThreadAsDaemon)(JavaVM*, JNIEnv**, void*);
};

/*
 * C++ version.
 */
struct _JavaVM {
    const struct JNIInvokeInterface* functions;

#if defined(__cplusplus)
    jint DestroyJavaVM()
    {
        return functions->DestroyJavaVM(this);
    }
    jint AttachCurrentThread(JNIEnv** p_env, void* thr_args)
    {
        return functions->AttachCurrentThread(this, p_env, thr_args);
    }
    jint DetachCurrentThread()
    {
        return functions->DetachCurrentThread(this);
    }
    jint GetEnv(void** env, jint version)
    {
        return functions->GetEnv(this, env, version);
    }
    jint AttachCurrentThreadAsDaemon(JNIEnv** p_env, void* thr_args)
    {
        return functions->AttachCurrentThreadAsDaemon(this, p_env, thr_args);
    }
#endif /*__cplusplus*/
};

struct JavaVMAttachArgs {
    jint        version;    /* must be >= JNI_VERSION_1_2 */
    const char* name;       /* NULL or name of thread as modified UTF-8 str */
    jobject     group;      /* global ref of a ThreadGroup object, or NULL */
};
typedef struct JavaVMAttachArgs JavaVMAttachArgs;

/*
 * JNI 1.2+ initialization.  (As of 1.6, the pre-1.2 structures are no
 * longer supported.)
 */
typedef struct JavaVMOption {
    const char* optionString;
    void* extraInfo;
} JavaVMOption;

typedef struct JavaVMInitArgs {
    jint        version;    /* use JNI_VERSION_1_2 or later */

    jint        nOptions;
    JavaVMOption* options;
    jboolean    ignoreUnrecognized;
} JavaVMInitArgs;

#ifdef __cplusplus
extern "C" {
#endif
    /*
     * VM initialization functions.
     *
     * Note these are the only symbols exported for JNI by the VM.
     */
    jint JNI_GetDefaultJavaVMInitArgs(void*);
    jint JNI_CreateJavaVM(JavaVM**, JNIEnv**, void*);
    jint JNI_GetCreatedJavaVMs(JavaVM**, jsize, jsize*);

#define JNIIMPORT
#define JNIEXPORT  __attribute__ ((visibility ("default")))
#define JNICALL

    /*
     * Prototypes for functions exported by loadable shared libs.  These are
     * called by JNI, not provided by JNI.
     */
    JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved);
    JNIEXPORT void JNI_OnUnload(JavaVM* vm, void* reserved);

#ifdef __cplusplus
}
#endif


/*
 * Manifest constants.
 */
#define JNI_FALSE   0
#define JNI_TRUE    1

#define JNI_VERSION_1_1 0x00010001
#define JNI_VERSION_1_2 0x00010002
#define JNI_VERSION_1_4 0x00010004
#define JNI_VERSION_1_6 0x00010006

#define JNI_OK          (0)         /* no error */
#define JNI_ERR         (-1)        /* generic error */
#define JNI_EDETACHED   (-2)        /* thread detached from the VM */
#define JNI_EVERSION    (-3)        /* JNI version error */
#define JNI_ENOMEM      (-4)        /* Out of memory */
#define JNI_EEXIST      (-5)        /* VM already created */
#define JNI_EINVAL      (-6)        /* Invalid argument */

#define JNI_COMMIT      1           /* copy content, do not free buffer */
#define JNI_ABORT       2           /* free buffer w/o copying back */

#endif  // LIBNATIVEHELPER_INCLUDE_JNI_JNI_H_

#endif /*Determine whether code gets compiled for Windows or Linux*/
#endif /*None of this gets compiled if __PEN_MOBILE__ is not defined*/