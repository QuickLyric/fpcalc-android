#include <jni.h>
#include <stdlib.h>
#include <cstring>
#include <vector>
#include <android/log.h>

#define LOGI(...)   __android_log_print((int)ANDROID_LOG_INFO, "CHROMAPRINT", __VA_ARGS__)

extern int fpcalc_main(int argc, char **argv);

static char *retval = NULL;
char * append_string(char * old, const char * newone)
{
    const size_t old_len = old ? strlen(old) : 0;
    const size_t newone_len = strlen(newone);
    const size_t out_len = old_len + newone_len + 1;
    char *out = (char*)malloc(out_len);
    if (old_len) memcpy(out, old, old_len);
    memcpy(out + old_len, newone, newone_len + 1);
    if (old_len) free(old);
    return out;
}

void jni_output(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    int size = vsnprintf(NULL, 0, format, argptr);
    char buffer[size + 1];
    vsprintf(buffer, format, argptr);
    retval = append_string(retval,buffer);
    va_end(argptr);
}

extern "C"
JNIEXPORT jstring

JNICALL
Java_com_geecko_fpcalc_FpCalc_fpCalc(JNIEnv *env, jobject thiz, jobjectArray args) {
    int i;
    int argc = env->GetArrayLength(args) + 1;
    char **argv = new char*[argc];
    std::vector<char*> argvadd;
    argv[0] = new char[7];
    strcpy(argv[0],"fpCalc");
    argvadd.push_back(argv[0]);
    retval = NULL;
    for (i=1; i<argc; i++)
    {
        jstring js = (jstring)env->GetObjectArrayElement(args,i-1);
        const char *cs = env->GetStringUTFChars(js, 0);
        argv[i] = new char[strlen(cs) + 1];
        strcpy(argv[i],cs);
        env->ReleaseStringUTFChars(js, cs);
        env->DeleteLocalRef(js);
        argvadd.push_back(argv[i]);
    }
    int rslt = fpcalc_main(argc,argv);

    if (rslt == 1)
    {
        jni_output("error_fpcalc_main=%d\n",rslt);
    }
    jstring final_result_value = env->NewStringUTF(retval);
    delete retval;
    retval = NULL;

    for (i=0; i<argc; i++)
    {
        delete[] argvadd[i];
    }
    delete[] argv;

    return final_result_value;
}
