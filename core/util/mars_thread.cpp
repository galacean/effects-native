#include "mars_thread.h"
#include <thread>
#include <sstream>
#include "util/log_util.hpp"
#include "util/util.hpp"
#include "util/constant.hpp"
#ifdef ANDROID
#include "platform/android/jni_bridge.h"
#endif

namespace mn {

static std::atomic<intptr_t> s_tids(0);
static pthread_key_t s_thread_key = 0;
static pthread_key_t s_thread_ptr_key = 0;
static pthread_once_t s_pthread_once = PTHREAD_ONCE_INIT;
static int s_thread_count = 0;

static void ThreadTidInitOnce() {
    pthread_key_create(&s_thread_key, NULL);
    pthread_key_create(&s_thread_ptr_key, NULL);
}

MarsThread::MarsThread(float dt, MarsThreadTickEvent* raf_event, bool lock_tick_event, const MarsConfig& config) {
    running_ = 1;
    paused_ = 0;
    dying_ = 0;
    dt_ = dt;
    raf_event_ = raf_event;
    lock_tick_event_ = lock_tick_event;

    enable_will_paused_ = config.enable_will_pause;
    will_paused_ = 0;
    MLOGD("MarsThread Constructed enable_will_pause: %d", enable_will_paused_);

    pthread_create(&pt_, NULL, StaticLoop, this);
}

MarsThread::~MarsThread() {
    MLOGD("MThread Destructed free %p", this);
    
    for (auto evt : event_list_) {
        delete evt;
    }
    
    pthread_detach(pt_);
    MLOGD("MThread Destructed free end %p", this);
}

void MarsThread::Post(MarsThreadEvent* evt) {
    std::unique_lock<std::mutex> lock(event_mutex_);
    event_list_.push_back(evt);
    if (evt->IsDestroyEvent()) {
        dying_ = 1;
    }
}

void MarsThread::Pause() {
    if (enable_will_paused_) {
        will_paused_ = 1;
    }
    MLOGD("MThread Pause %d", will_paused_.load());
    std::unique_lock<std::recursive_mutex> lock(pause_mutex_);
    paused_ = 1;
}

void MarsThread::Resume() {
    MLOGD("MThread Resume");
    std::unique_lock<std::recursive_mutex> lock(pause_mutex_);
    paused_ = 0;
    will_paused_ = 0;
    pause_condition_.notify_all();
}

// called from MarsThread
void MarsThread::Stop() {
    if (running_ > 0) {
        MLOGD("MThread start stop");
        running_ = 0;
        paused_ = 0;
        raf_event_ = nullptr;
        Resume();
    }
}

// called from MarsThread
void MarsThread::OnPlayStart() {
    last_update_time_ = Utils::TimestampMs();
}

void MarsThread::Loop() {
    // 绑定JNI环境，生成线程ID
    intptr_t tid = GetThreadId();
    ++s_thread_count;
    MLOGD("MThread attached %p %s %d", tid, MN_THREAD_NAME, s_thread_count);
#ifdef ANDROID
    pthread_setname_np(pt_, MN_THREAD_NAME);
#else
    pthread_setname_np(MN_THREAD_NAME);
#endif
    
    pthread_setspecific(s_thread_ptr_key, (void*)this);

    last_update_time_ = Utils::TimestampMs();
    int64_t except_frame_end_time = last_update_time_;

    while (running_) {
        except_frame_end_time += dt_;
        {
            std::unique_lock<std::recursive_mutex> lock(pause_mutex_);
            if (paused_ > 0) {
                MLOGD("MThread before pause");
                pause_condition_.wait(lock, [this] {
                    return paused_ <= 0;
                });
                MLOGD("MThread after pause");
            }
        }
        if (running_ == 0) {
            break;
        }

        std::vector<MarsThreadEvent*> temp;
        {
            std::unique_lock<std::mutex> lock(event_mutex_);
            if (!event_list_.empty()) {
                temp.swap(event_list_);
            }
        }
        if (!temp.empty()) {
            for (MarsThreadEvent* evt : temp) {
                if (!dying_ || evt->IsDestroyEvent()) {
                    if (lock_tick_event_ && evt->IsTickEvent()) {
                        std::unique_lock<std::recursive_mutex> lock(pause_mutex_);
                        if (!paused_) {
                            evt->Run();
                        } else {
                            MLOGD("MThread ignore tick event on paused");
                        }
                    } else {
                        evt->Run();
                    }
                }
                delete evt;
            }
        }

        int64_t current = Utils::TimestampMs();
        if (raf_event_ && !dying_) {
            if (lock_tick_event_) {
                std::unique_lock<std::recursive_mutex> lock(pause_mutex_);
                if (!paused_) {
                    raf_event_->UpdateRenderFrame(current - last_update_time_);
                } else {
                    MLOGD("MThread ignore raf_event_ on paused");
                }
            } else {
                raf_event_->UpdateRenderFrame(current - last_update_time_);
            }
            // raf之后要更新current
            last_update_time_ = current;
            current = Utils::TimestampMs();
        }
        if (current < except_frame_end_time) {
            std::this_thread::sleep_for(std::chrono::milliseconds(except_frame_end_time - current));
        }
    }
    --s_thread_count;
    MLOGD("MThread finish %d", s_thread_count);
}

intptr_t MarsThread::GetThreadId() {
    pthread_once(&s_pthread_once, ThreadTidInitOnce);
    intptr_t tid = (intptr_t)pthread_getspecific(s_thread_key);
    if (tid == 0) {
        tid = ++s_tids;
#ifdef ANDROID
        // 解决JNI调用问题
        JNIUtil::GetEnv();
#endif
        pthread_setspecific(s_thread_key, (void*)tid);
    }
    return tid;
}

// 只在gl线程调用
bool MarsThread::GetThreadPaused() {
    if (!s_thread_ptr_key) {
        return false;
    }
    void* ptr = pthread_getspecific(s_thread_ptr_key);
    if (!ptr) {
        return false;
    }
    MarsThread* t = (MarsThread*)ptr;
    bool ret = (t->paused_ > 0) || (t->dying_ > 0) || (t->will_paused_ > 0);
    if (ret) {
        MLOGE("mars thread is paused");
    }
    return ret;
}

void* MarsThread::StaticLoop(void* context) {
    MarsThread* thread = (MarsThread*) context;
    thread->Loop();
    MLOGD("MThread finish stop");
    delete thread;
    return nullptr;
}

}
