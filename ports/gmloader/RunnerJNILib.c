#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "fake_jni.h"
#include "so_util.h"
#include "RunnerJNILib.h"
#include "so_util.h"
#include "libyoyo.h"
#include "libyoyo_internals.h"

#define MANGLED_CLASSPATH "Java_com_yoyogames_runner_RunnerJNILib_"
#define CLASSPATH "com/yoyogames/runner/RunnerJNILib"
#define CLASSNAME "RunnerJNILib"
#define CLASS _jclass RunnerJNILib_class
#define STRUCT_NAME RunnerJNILib_obj
#define NATIVE_LIB_FUNCS JNIRUNNER_NATIVE_LIB_FUNCS
#define MANAGED_LIB_FUNCS JNIRUNNER_MANAGED_LIB_FUNCS
#define RESOLVER Resolve_RunnerJNILib
#define FIELDS JNIRUNNER_FIELDS

#include "prelude_helpers.h"

static void RunnerJNILib_OpenURL(jstring url)
{
    WARN_STUB
}

static int RunnerJNILib_OsGetInfo()
{
    _jstring osinfo_arr[] = {
        /* "RELEASE", */           MK_JSTRING("v1.0"),
        /* "MODEL", */             MK_JSTRING("Homebrew"),
        /* "DEVICE", */            MK_JSTRING("Homebrew"),
        /* "MANUFACTURER", */      MK_JSTRING("JohnnyonFlame"),
        /* "CPU_ABI", */           MK_JSTRING("armeabi"),
        /* "CPU_ABI2", */          MK_JSTRING("armeabi-v7a"),
        /* "BOOTLOADER", */        MK_JSTRING("U-Boot"),
        /* "BOARD", */             MK_JSTRING("You tell me"),
        /* "VERSION", */           MK_JSTRING("v1.0"),
        /* "REGION", */            MK_JSTRING("Global"),
        /* "VERSION_NAME", */      MK_JSTRING("v1.0"),
    };

    ENSURE_SYMBOL(libyoyo, CreateDsMap, "_Z11CreateDsMapiz");
    int osinfo = RunnerJNILib_CreateVersionDSMap(jni_get_env(), NULL, 0x13,
        &osinfo_arr[0], &osinfo_arr[1], &osinfo_arr[2], &osinfo_arr[3], &osinfo_arr[4], &osinfo_arr[5],
        &osinfo_arr[6], &osinfo_arr[7], &osinfo_arr[8], &osinfo_arr[9], &osinfo_arr[10], (jboolean)1);
    warning(" -- Retuning OsInfo %d. --\n", osinfo);
    return osinfo;
}

static jobject RunnerJNILib_CallExtensionFunction1(jstring class, jstring method, jint count, jdoubleArray dblarray, jobjectArray args)
{
    WARN_STUB
}

static jobject RunnerJNILib_CallExtensionFunction2(jstring class, jstring method, jint argcount, jobjectArray args)
{
    WARN_STUB
}

static jint RunnerJNILib_GamepadsCount()
{
    int count = 0;
    for (int i = 0; i < ARRAY_SIZE(yoyo_gamepads); i++)
        if (yoyo_gamepads[i].is_available)
            count++;

    return count;
}

static void RunnerJNILib_ClearGamepads()
{
    WARN_STUB
}

static jboolean RunnerJNILib_GamepadConnected(jint deviceIndex)
{
    if (deviceIndex >= ARRAY_SIZE(yoyo_gamepads))
        return JNI_FALSE;

    return yoyo_gamepads[deviceIndex].is_available;
}

static jstring RunnerJNILib_GamepadDescription(jint deviceIndex)
{
    jstring ret = malloc(sizeof(_jstring));
    *ret = MK_JSTRING(strdup("Generic Gamepad"));
    return ret;
}

static jfloatArray RunnerJNILib_GamepadAxesValues(jint deviceIndex)
{
    WARN_STUB
}

static jfloatArray RunnerJNILib_GamepadButtonValues(jint deviceIndex)
{
    WARN_STUB
}

static jint RunnerJNILib_GamepadGMLMapping(jint deviceIndex, jint inputId)
{
    WARN_STUB
}

static void RunnerJNILib_PlayMP3(jstring file, jint loops)
{
    WARN_STUB
}

static void RunnerJNILib_StopMP3()
{
    WARN_STUB
}

static void RunnerJNILib_PauseMP3()
{
    WARN_STUB
}

static void RunnerJNILib_ResumeMP3()
{
    WARN_STUB
}

static void RunnerJNILib_SetMP3Volume(jfloat vol)
{
    WARN_STUB
}

static jboolean RunnerJNILib_PlayingMP3()
{
    WARN_STUB
}

static jint RunnerJNILib_GetDefaultFrameBuffer()
{
    // TODO:: Seems to only be set when in GL1.1 mode, investigate.
    WARN_STUB
    return -1;
}

static void RunnerJNILib_ShowMessage(jstring msg)
{
    WARN_STUB
}

static jint RunnerJNILib_UsingGL2()
{
    WARN_STUB
    return 1;
}

static jstring RunnerJNILib_GetUDID()
{
    jstring udid = malloc(sizeof(_jstring));
    *udid = MK_JSTRING(strdup("GetUDID"));
    return udid;
}

static void RunnerJNILib_WaitForVsync()
{
    ;
}

static jint RunnerJNILib_HasVsyncHandler()
{
    return 0;
}

#include "prologue_helpers.h"