//
//  log_util.hpp
//
//  Created by Zongming Liu on 2021/11/9.
//

#ifndef log_util_hpp
#define log_util_hpp

#include <stdio.h>
#include <assert.h>
#include <string>
#include <vector>

#ifndef GLOBAL_MN_LOG_TAG
#define GLOBAL_MN_LOG_TAG "MN::"
#endif

#ifndef MLOGD
//#define ALOGI(...) ((void)ag::LogUtil::native_clog(ag::LOG_LEVEL_INFO, GLOBAL_AG_LOG_TAG, __VA_ARGS__))
#define MLOGD(...) ((void)mn::LogUtil::native_clog(mn::LOG_LEVEL_INFO, GLOBAL_MN_LOG_TAG, __VA_ARGS__))
#endif

#ifndef MLOGE
//#define ALOGI(...) ((void)ag::LogUtil::native_clog(ag::LOG_LEVEL_INFO, GLOBAL_AG_LOG_TAG, __VA_ARGS__))
#define MLOGE(...) ((void)mn::LogUtil::native_clog(mn::LOG_LEVEL_ERROR, GLOBAL_MN_LOG_TAG, __VA_ARGS__))
#endif

//#ifdef DEBUG
//#define DEBUG_GL_DATA 1
//#endif

#ifdef DEBUG_GL_DATA
#define DEBUG_DATA(tag, data, len, bpe) LogUtil::DebugDataLog(tag, data, len, bpe)
#define DEBUG_DATA_STRIDE(tag, data, len, bpe, stride) LogUtil::DebugDataStrideLog(tag, data, len, bpe, stride)
#define DEBUG_VECTOR(tag, data, index, len) LogUtil::DebugVector(tag, data, index, len)
#define DEBUG_MLOGD(...) MLOGD(__VA_ARGS__);
#else
#define DEBUG_DATA(tag, data, len, bpe)
#define DEBUG_DATA_STRIDE(tag, data, len, bpe, stride)
#define DEBUG_VECTOR(tag, data, index, len)
#define DEBUG_MLOGD(...)
#endif

namespace mn {

typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_VERBOSE,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} LogLevel;

class LogUtil {
  
public:
    
    static void native_clog(LogLevel level, const char* tag, const char *fmt, ...);
    
    static void platform_log(int level, const char* msg);
    
#ifdef DEBUG_GL_DATA
    
    static void DebugDataStrideLog(const std::string& tag, void* data, size_t byte_length, size_t byte_per_element, size_t stride);
    
    static void DebugDataLog(const std::string& tag, void* data, size_t byte_length, size_t byte_per_element);
    
    static void DebugVector(const std::string& tag, const std::vector<float>& data, size_t index, size_t length);

#endif
};

}

#endif /* log_util_hpp */
