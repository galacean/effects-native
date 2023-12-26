#ifndef MN_MARS_THREAD_H_
#define MN_MARS_THREAD_H_

#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <vector>
#include "util/config.h"

namespace mn {
// todo: 改为std::function+lambda
// MarsThreadEvent里的成员都不需要考虑释放成员变量的线程问题，在析构里处理即可，MarsThread退出时会释放所有event
class MarsThreadEvent {
public:
    MarsThreadEvent() {}

    virtual ~MarsThreadEvent() {}

    virtual void Run() = 0;

    virtual bool IsDestroyEvent() { return false; }

    virtual bool IsTickEvent() { return false; }
};

class MarsThreadTickEvent {
public:
    virtual void UpdateRenderFrame(int dt) = 0;
};

class MarsThread {
public:
    MarsThread(float dt, MarsThreadTickEvent* raf_event, bool lock_tick_event, const MarsConfig& config);

    void Post(MarsThreadEvent* evt);

    void Pause();

    void Resume();
    
    void Stop();
    
    void OnPlayStart();

    static intptr_t GetThreadId();
    
    static bool GetThreadPaused();
    
protected:
    
    virtual ~MarsThread();

private:
    void Loop();

    static void* StaticLoop(void* context);

private:
    pthread_t pt_;

    std::mutex event_mutex_;
    std::vector<MarsThreadEvent*> event_list_;

    MarsThreadTickEvent* raf_event_ = nullptr;

    std::atomic<int> running_;

    std::recursive_mutex pause_mutex_;
    std::atomic<int> paused_;
    std::condition_variable_any pause_condition_;

    bool enable_will_paused_;
    std::atomic<int> will_paused_;

    std::atomic<int> dying_;

    int dt_;
    bool lock_tick_event_ = false;
    
    int64_t last_update_time_ = 0;
};

}

#endif
