#include "../../../../../../../src/state/config.h"

#ifdef __PEN_MOBILE__

#ifndef _JAVASOFT_JNI_MD_H_
#define _JAVASOFT_JNI_MD_H_

#define JNIEXPORT __declspec(dllexport)
#define JNIIMPORT __declspec(dllimport)
#define JNICALL __stdcall

#ifndef __PEN_LINUX__
/*----For Windows----*/
// 'long' is always 32 bit on windows so this matches what jdk expects
typedef long jint;
typedef __int64 jlong;
typedef signed char jbyte;
/*----For Windows----*/
#else
/*----For Linux----*/
typedef long jint;
#ifdef __GNUC__
typedef long long jlong;
#else
typedef __int64 jlong;
#endif
typedef signed char jbyte;
/*----For Linux----*/
#endif


#endif /* !_JAVASOFT_JNI_MD_H_ */
#endif