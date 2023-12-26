//
//  log_util.cpp
//
//  Created by Zongming Liu on 2021/11/9.
//

#include "log_util.hpp"
#include "ri/gl.h"
#include <sstream>

#ifdef ANDROID
#include <android/log.h>
#endif

void CheckGLError(const char* stmt, const char* fname, int line) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        MLOGE("[AGSurface]:OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
//        abort();
    }
}

namespace mn {

void LogUtil::native_clog(LogLevel level, const char* tag, const char *fmt, ...) {
    
    int length = 256;
    char buffer[length];
    va_list args;
    va_start (args, fmt);
    vsnprintf (buffer, length, fmt, args);
    va_end (args);
    buffer[length - 1] = '\0';
    
    char* tt = const_cast<char*>(tag);
    char* data = const_cast<char*>(buffer);
    
//#ifdef ANDROID
//    auto l = ANDROID_LOG_DEBUG;
//    switch (level) {
//    case LOG_LEVEL_WARN:
//        l = ANDROID_LOG_WARN;
//        break;
//    
//    case LOG_LEVEL_ERROR:
//        l = ANDROID_LOG_ERROR;
//        break;
//    
//    default:
//        break;
//    }
//    __android_log_print(l, tt, "%s", data);
//#else
//    // todo: 后续转化NSLog；
//    printf("[CLOG] [%s] %s\n", tt, data);
//#endif
    platform_log((int) level, data);
}

#ifdef DEBUG_GL_DATA
void LogUtil::DebugDataLog(const std::string& tag, void *data, size_t byte_length, size_t byte_per_element) {
    if (data != nullptr) {
        size_t debug_len = byte_length / byte_per_element;
        std::stringstream ss;
        for (size_t i=0; i<debug_len; i++) {
            if (byte_per_element == 4) {
                float tmpData = ((float *)(data))[i];
                ss << tmpData << ",";
            } else if (byte_per_element == 2) {
                uint16_t tmpData = ((uint16_t *)(data))[i];
                ss << tmpData << ",";
            } else {
                MLOGD("DebugData not support data type");
            }
        }
        MLOGD("DebugData %s: Data Length %d, Data: %s", tag.data(), debug_len, ss.str().data());
    }
}

void LogUtil::DebugDataStrideLog(const std::string& tag, void* data, size_t byte_length, size_t byte_per_element, size_t byte_stride) {
    if (data != nullptr) {
        size_t debug_len = byte_length / byte_stride;
        size_t stride_len = byte_stride / byte_per_element;
        std::stringstream ss;
        for (size_t i=0; i<debug_len; i++) {
            if (byte_per_element == 4) {
                float tmpData = ((float *)(data))[i * stride_len];
                ss << tmpData << ",";
            } else if (byte_per_element == 2) {
                uint16_t tmpData = ((uint16_t*)(data))[i * stride_len];
                ss << tmpData << ",";
            } else {
                MLOGD("DebugDataStide not support data type");
            }
        }
        MLOGD("DebugDataStride %s :Data Length %d, Stride: %d, Data: %s", tag.data(), debug_len, stride_len, ss.str().data());
    }
}

void LogUtil::DebugVector(const std::string& tag, const std::vector<float>& data, size_t index, size_t length) {
    std::stringstream ss;
    for (size_t i=0; i<length; i++) {
        ss << data[index + i] << ",";
    }
    
    MLOGD("DebugVectorData %s: DataLength %d, Data is: %s", tag.data(), length, ss.str().data());
}

#endif

}
