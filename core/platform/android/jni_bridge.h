#ifndef MN_JNI_BRIDGE_H_
#define MN_JNI_BRIDGE_H_

#include <jni.h>
#include <string>

namespace mn {

class JNIUtil {
public:
    static void SetJavaVM(JavaVM* javaVM);

    static JNIEnv* GetEnv();

    static jclass FindClass(const char* name);

    static jclass GetEventEmiterClass();

    static jmethodID GetEventEmiterOnEventMID();

    static jmethodID GetEventEmiterOnLogMID();

    static jmethodID GetEventEmiterParseBitmapMID();

    static jclass GetVideoSurfaceUtilClass();
};

}

#endif
