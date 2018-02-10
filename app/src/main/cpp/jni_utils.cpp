//------------------------------------------------------------------
// jni_utils.c
//------------------------------------------------------------------
//
// Generic routines to allow a library to link itself
// with the java class that called it, instead of requiring
// the library to know what class it will be linked to.
//
// YOUR LIBRARY MUST PROVIDE THE ARRAY OF FUNCTIONS TO
// REGISTER AND THE SIZE OF THAT ARRAY:
//
//    JNINativeMethod library_methods[] = {}
//    jsize library_methods_size = sizeof(library_methods)/sizeof(JNINativeMethod);
//
// Otherwise, by merely linking this C code into your JNI
// shared library, it will automatically be "linked" to
// whatever class calls "LoadLibrary()" on it.
//
//-----------------------------------------------------------------
//
// This file is distributed under the GNU GENERAL PUBLIC LICENSE version 2.
// Please see the file `COPYING.txt` for details.
//
// You are free to modify it, and distribute in any way you see fit,
// as long as you retain this notification and all copyrights herein.
//
// This software is provided without any warranty, explicit or implied.
//
// (c) Copyright 2015 - Patrick Horton


//------------------------------------------------------
// What follows is General Notes on JNI type specifiers
//------------------------------------------------------
// to help you create your methods array ...
//
// JNINativeMethod defined as { name, signature, fnPtr }
//
//     signature == (param_types)return_type
//
// Character Java Type C Type
//
//     V void void
//     Z jboolean boolean
//     I jint int
//     J jlong ??long
//     D jdouble double
//     F jfloat float
//     The B jbyte byte
//     The C jchar char
//     S jshort short
//
// Array by the "[" start, the two characters
//
//     [I jintArray int []
//     [The F jfloatArray float []
//     [B jbyteArray byte []
//     [C jcharArray char []
//     [The S jshortArray Short []
//     [The D jdoubleArray double []
//     [J jlongArray long []
//     [Z jbooleanArray boolean []
//
// Class parameters start with L and are delimited with ";"
//
//     Ljava/lang/String;  == String
//     Ljava/net/Socket; == Socket
//
// If the the JAVA function is an embedded class, $ as the class name separator.
//
// 		Landroid/os/FileUtils$ FileStatus ;) Z "


#include <stdio.h>
#include <stdlib.h>
#include <jni.h>
#include <cstring>


#ifdef JNI_LOGGING
#include <android/log.h>
	#define jni_log(...) __android_log_print(ANDROID_LOG_DEBUG, "stdjni", __VA_ARGS__)
#else
#define jni_log(...)
#endif

// YOU MUST PROVIDE lbrary_methods[] and library_methods_size

extern JNINativeMethod library_methods[];
extern jsize library_methods_size;



//--------------------------------------------------------
// utilities
//--------------------------------------------------------

static int my_strcmpn(const char *s, const char *t, int n)
// decrement j and if strings are equal
// it will be zero
{
    int i, j;

    j = n;
    i = 0;
    while (i < n)
    {
        if (s[i] == t[i])
        {
            j--;
        }
        i++;
    }
    return j;
}


//--------------------------------------------------------
// generic getLoaderClass
//--------------------------------------------------------

char *getLoaderClass(JNIEnv* env)
// Returns the fully qualified path to the class that
// called loadLibrary with this shared library.
//
// This can be used to relieve the library of knowing that
// class that it will be linked to, instead of embedding
// funky /com/whatever/someClassName constants in your C
//
// It works by getting the Thread class and calling it's
// currentThread() static method to get the current_thread,
// then calling getStackTrace() on it to get java call stack,
// and then looping thru the call stack, it finds the first
// element that is not in the "java/lang" call chain, which
// is presumed to be the class that called System.loadlibrary()!
{
    jni_log("getLoaderClass called");
    char *the_class_name = NULL;

    jclass thread_class = env->FindClass("java/lang/Thread");
    if (!thread_class)
    {
        jni_log("getLoaderClass Could not find /java/lang/Thread class");
        goto done;
    }
    jclass stack_trace_element_class = env->FindClass("java/lang/StackTraceElement");
    if (!stack_trace_element_class)
    {
        jni_log("getLoaderClass Could not find java/lang/StackTraceElement class");
        goto done;
    }
    jmethodID getClassName = env->GetMethodID(stack_trace_element_class, "getClassName", "()Ljava/lang/String;");
    if (!getClassName)
    {
        jni_log("getLoaderClass Could not get java/lang/StackTraceElement/getClassName() method");
        goto done;
    }

    // get the current thread and call getStackTrace

    jmethodID currentThread = env->GetStaticMethodID(thread_class,
                                                        "currentThread", "()Ljava/lang/Thread;");
    if (!currentThread)
    {
        jni_log("getLoaderClass Could not get java/lang/Thread/currentThread() method");
        goto done;
    }
    jni_log("getLoaderClass Calling java/lang/Thread/currentThread()");
    jobject current_thread = env->CallStaticObjectMethod(thread_class,currentThread);
    jni_log("getLoaderClass Back from calling java/lang/Thread/currentThread()");
    if (!currentThread)
    {
        jni_log("getLoaderClass Could not get current_thread");
        goto done;
    }

    jmethodID getStackTrace = env->GetMethodID(thread_class,
                                                  "getStackTrace",
                                                  "()[Ljava/lang/StackTraceElement;");
    if (!getStackTrace)
    {
        jni_log("getLoaderClass Could not get java/lang/Thread/getStackTrace() method");
        goto done;
    }
    jni_log("getLoaderClass Calling java/lang/Thread/getStackTrace()");
    jarray stack_trace = (jarray) env->CallObjectMethod(current_thread, getStackTrace);
    jni_log("getLoaderClass Back from calling java/lang/Thread/getStackTrace()");
    if (!stack_trace)
    {
        jni_log("getLoaderClass Could not get stack_trace");
        goto done;
    }

    //----------------------------
    // loop thru the trace
    //----------------------------

    jsize trace_len = env->GetArrayLength(stack_trace);
    jni_log("getLoaderClass Trace has %d elements",trace_len);

    // jsize=1 to skip the call to getStackTrace itself

    jsize i;
    jobject trace_element = NULL;
    jstring j_class_name = NULL;
    const char *class_name = NULL;

    for (i=1; i<trace_len; i++)
    {
        trace_element = env->GetObjectArrayElement( (jobjectArray)stack_trace, i);
        if (!trace_element)
        {
            jni_log("getLoaderClass     could not get trace_element(%d)",i);
            break;
        }
        j_class_name = (jstring) env->CallObjectMethod(trace_element,getClassName);
        if (!j_class_name)
        {
            jni_log("getLoaderClass     could not get trace_element j_class_name(%d)",i);
            break;
        }
        class_name = env->GetStringUTFChars(j_class_name,0);
        if (!class_name)
        {
            jni_log("getLoaderClass     could not get convert trace_element j_class_name(%d) to char *",i);
            break;
        }
        jni_log("getLoaderClass     class_name(%d) = %s",i,class_name);

        if (my_strcmpn(class_name,"java.lang.",10))
        {
            jni_log("getLoaderClass     FOUND the_class_name(%d)=%s",i,class_name);
            the_class_name = (char*)malloc(strlen(class_name)+1);
            strcpy(the_class_name,class_name);
            break;
        }

        env->ReleaseStringUTFChars(j_class_name,class_name);
        env->DeleteLocalRef(trace_element);

        trace_element = NULL;
        j_class_name = NULL;
        class_name = NULL;

    }

    // finished

    if (!the_class_name)
    {
        jni_log("getLoaderClass Could not find the_class_name of activity to link to");
    }
    else	// convert '.' to '/' in the_class_name
    {
        for (i=0; i<strlen(the_class_name); i++)
        {
            if (the_class_name[i] == '.')
            {
                the_class_name[i] = '/';
            }
        }
        jni_log("getLoaderClass returning %s",the_class_name);
    }

    done:	// cleanup (all local object references)

    if (trace_element)
        env->DeleteLocalRef(trace_element);
    if (class_name)
        env->ReleaseStringUTFChars(j_class_name,class_name);
    if (j_class_name)
        env->DeleteLocalRef(j_class_name);
    if (stack_trace)
        env->DeleteLocalRef(stack_trace);
    if (current_thread)
        env->DeleteLocalRef(current_thread);
    if (stack_trace_element_class)
        env->DeleteLocalRef(stack_trace_element_class);
    if (thread_class)
        env->DeleteLocalRef(thread_class);

    return the_class_name;
}


//----------------------------------------------------------------
// Normal JNI_OnLoad() but using determined class name
//----------------------------------------------------------------

jint JNI_OnLoad(JavaVM* vm, void* reserved)
// This could be a useful function to other folks,
// in that it relieves the library of knowing the
// class that is loading it ...
{
    JNIEnv* env = NULL;
    jint result = JNI_ERR;
    jni_log("JNI_OnLoad() called");

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK)
    {
        jni_log("JNI_ONLOAD GetEnv() failed");
        goto done;
    }

    char *the_class_name = getLoaderClass(env);
    // instead of /com/whatever/some_funky_class_name_in_my_c_code
    if (!the_class_name)
    {
        jni_log("JNI_ONLOAD getLoaderClass() failed");
        goto done;
    }

    jclass activity_class = env->FindClass(the_class_name);
    if (!activity_class)
    {
        jni_log("JNI_OnLoad() FindClass(%s) failed",the_class_name);
        goto done;
    }

    // normal ending

    env->RegisterNatives( activity_class, library_methods, library_methods_size );
    jni_log("JNI_OnLoad(%s) succeeded",the_class_name);

    done:

    if (the_class_name)
        free(the_class_name);
    if (activity_class)
        env->DeleteLocalRef(activity_class);

    return JNI_VERSION_1_4;
}

