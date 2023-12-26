#include "jni_bridge.h"
#include <string>
#include <pthread.h>
#include "util/log_util.hpp"
#include "mars_player_jni.h"
#include "util/image_loader.h"

static jobject g_class_loader;
static jmethodID g_load_class_method;

static jclass g_event_emitter_clazz = nullptr;
static jmethodID g_on_event_mid = nullptr;
static jmethodID g_on_log_mid = nullptr;
static jmethodID g_parse_bitmap_mid = nullptr;

static jclass g_video_surface_util_clazz = nullptr;

namespace mn {

static JavaVM* s_java_vm = nullptr;
static pthread_key_t s_jenv_threadKey;

static void detach_current_thread(void* env) {
    s_java_vm->DetachCurrentThread();
}

void JNIUtil::SetJavaVM(JavaVM* javaVM) {
    s_java_vm = javaVM;
    pthread_key_create(&s_jenv_threadKey, detach_current_thread);
}

JNIEnv* JNIUtil::GetEnv() {
    JNIEnv* env = (JNIEnv*)pthread_getspecific(s_jenv_threadKey);
    if (!env) {
        int status = s_java_vm->GetEnv((void**)&env, JNI_VERSION_1_6);
        switch (status) {
            case JNI_EDETACHED:
                if (s_java_vm->AttachCurrentThread(&env, 0) < 0) {
                    MLOGE("Failed to get the environment using AttachCurrentThread()");
                    env = NULL;
                    break;
                }
                // fall through
            case JNI_OK:
                pthread_setspecific(s_jenv_threadKey, env); 
                break;
            default:
                env = NULL;
                MLOGE("Failed to get the environment using GetEnv()");
                break;
        }
    }
    return env;
}

jclass JNIUtil::FindClass(const char* name) {
    jclass result = nullptr;
    JNIEnv* env = JNIUtil::GetEnv();
    if (env) {
        // 如果是子线程中调用，这句会出错，出现找不到类错误，所以要处理错误
        result = env->FindClass(name);
        jthrowable exception = env->ExceptionOccurred();
        if (exception) {
            env->ExceptionClear();

            jstring name_jstr = env->NewStringUTF(name);
            auto find_class = static_cast<jclass>(env->CallObjectMethod(g_class_loader, g_load_class_method, name_jstr));
            env->DeleteLocalRef(name_jstr);
            return find_class;
        }
    }
    return result;
}

jclass JNIUtil::GetEventEmiterClass() {
    return g_event_emitter_clazz;
}

jmethodID JNIUtil::GetEventEmiterOnEventMID() {
    return g_on_event_mid;
}

jmethodID JNIUtil::GetEventEmiterOnLogMID() {
    return g_on_log_mid;
}

jmethodID JNIUtil::GetEventEmiterParseBitmapMID() {
    return g_parse_bitmap_mid;
}

jclass JNIUtil::GetVideoSurfaceUtilClass() {
    return g_video_surface_util_clazz;
}

}

static const std::string JNIUtil_clazz = "com/alipay/mobile/mars/util/JNIUtil";

static JNINativeMethod JNIUtil_methods[] = {
    {"nativeMarsCreate", "(IIZZ)V", (void *) mn::MarsPlayerJNI::CreateNew},
    {"nativeMarsAddPlugin", "(IJLjava/lang/String;)V", (void *) mn::MarsPlayerJNI::AddPlugin},
    {"nativeSetRepeatCount", "(II)V", (void *) mn::MarsPlayerJNI::SetRepeatCount},
    {"nativeSetupSurface", "(ILandroid/view/Surface;)Z", (void *) mn::MarsPlayerJNI::SetupSurface},
    {"nativeResizeSurface", "(III)V", (void*) mn::MarsPlayerJNI::ResizeSurface},
    {"nativeDestroySurface", "(I)V", (void *) mn::MarsPlayerJNI::DestroySurface},
    {"nativeSetSceneData", "(IJ)V", (void *) mn::MarsPlayerJNI::SetSceneData},
    {"nativeMarsPlay", "(ILjava/lang/String;Z)V", (void *) mn::MarsPlayerJNI::PlayMars},
    {"nativeMarsPlayFrameControl", "(IIILjava/lang/String;Z)V", (void *) mn::MarsPlayerJNI::PlayMarsFrameControl},
    {"nativeMarsStop", "(I)V", (void *) mn::MarsPlayerJNI::StopMars},
    {"nativeMarsDestroy", "(I)V", (void *) mn::MarsPlayerJNI::DestroyMars},
    {"nativeMarsEvent", "(II)V", (void *) mn::MarsPlayerJNI::OnEvent},
    {"nativeSceneDataCreate", "([BI)J", (void *) mn::MarsPlayerJNI::CreateSceneData},
    {"nativeSceneDataCreateByPath", "(Ljava/lang/String;)J", (void *) mn::MarsPlayerJNI::CreateSceneDataByPath},
    {"nativeSceneDataCreateBySingleBinData", "([BI)J", (void *) mn::MarsPlayerJNI::CreateSceneDataByBinData},
    {"nativeSceneDataCreateBySingleBinPath", "(Ljava/lang/String;)J", (void *) mn::MarsPlayerJNI::CreateSceneDataByBinPath},
    {"nativeSceneDataDestroy", "(J)V", (void *) mn::MarsPlayerJNI::DestroySceneData},
    {"nativeSceneDataSetLooping", "(JZ)V", (void *) mn::MarsPlayerJNI::SceneDataSetLooping},
    {"nativeSetSceneImageResource", "(JLjava/lang/String;[BI)V", (void *) mn::MarsPlayerJNI::SceneDataSetImageData},
    {"nativeSetSceneImageResourceBmp", "(JLjava/lang/String;Landroid/graphics/Bitmap;)Z", (void *) mn::MarsPlayerJNI::SceneDataSetBitmapData},
    {"nativeSetSceneImageResourceVideo", "(JLjava/lang/String;J)V", (void *) mn::MarsPlayerJNI::SceneDataSetVideoData},
    {"nativeSceneDataGetAspect", "(J)F", (void *) mn::MarsPlayerJNI::SceneDataGetAspect}
};

static jint registerNatives(JNIEnv* env, const std::string& class_name, const JNINativeMethod* g_methods, int numMethods) {
    jclass clazz;
    clazz = env->FindClass(class_name.c_str());
    if (clazz == nullptr) {
        MLOGE("Fatal error: failed to register natives for JSEngine");
        return JNI_ERR;
    }

    env->RegisterNatives(clazz, g_methods, numMethods);
    return JNI_TRUE;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    mn::JNIUtil::SetJavaVM(vm);
    JNIEnv *env;
    jint onLoad_err = -1;

    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        MLOGE("JNI_OnLoad error");
        return onLoad_err;
    }

    if (env == nullptr) {
        MLOGE("JNI_OnLoad no env");
        return onLoad_err;
    }
    MLOGD("JNI_OnLoad");

    jclass toolclass = env->FindClass("com/alipay/mobile/mars/EventEmitter");
    jclass classClass = env->GetObjectClass(toolclass);
    jclass classLoaderClass = env->FindClass("java/lang/ClassLoader");
    jmethodID getClassLoaderMethod = env->GetMethodID(classClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
    jobject localClassLoader = env->CallObjectMethod(toolclass, getClassLoaderMethod);
    g_class_loader = env->NewGlobalRef(localClassLoader);
    g_load_class_method = env->GetMethodID(classLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    g_event_emitter_clazz = (jclass) env->NewGlobalRef(toolclass);
    g_on_event_mid = env->GetStaticMethodID(g_event_emitter_clazz, "onEvent", "(IILjava/lang/String;)V");
    g_on_log_mid = env->GetStaticMethodID(g_event_emitter_clazz, "onLog", "(ILjava/lang/String;)V");
    g_parse_bitmap_mid = env->GetStaticMethodID(g_event_emitter_clazz, "parseBitmap", "([B)Landroid/graphics/Bitmap;");
    { // VideoSurfaceUtil
        jclass clazz = env->FindClass("com/alipay/mobile/mars/util/VideoSurfaceUtil");
        g_video_surface_util_clazz = (jclass) env->NewGlobalRef(clazz);
    }

    size_t nativeMethodSize = sizeof(JNINativeMethod);
    registerNatives(env, JNIUtil_clazz, JNIUtil_methods,
                    sizeof(JNIUtil_methods) / nativeMethodSize);

    return JNI_VERSION_1_6;
}