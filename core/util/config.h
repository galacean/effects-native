#ifndef MN_UTIL_CONFIG_H_
#define MN_UTIL_CONFIG_H_

namespace mn {

// todo: 10.3.10 开关收口到这里
struct MarsConfig {
    // 10.3.0，设为true后会在ui线程修改MarsThread.pause
#ifdef ANDROID
    bool enable_will_pause = false;
#else
    bool enable_will_pause = true;
#endif
    int render_level = 0x4;
    bool fix_tick_time = true;
};

}

#endif
