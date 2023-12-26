#include "mars_player_mgr.h"
#include "util/log_util.hpp"

namespace mn {

std::recursive_mutex MarsPlayerManager::s_player_mutex;
std::map<int, MarsPlayer*> MarsPlayerManager::s_player_map;

static std::mutex s_destroy_mutex;
static std::mutex s_start_mutex;

void MarsPlayerManager::CreateNew(int idx, MarsPlayerConstructor& options, bool native_tick) {
    std::lock_guard<std::recursive_mutex> lock(s_player_mutex);

    options.idx = idx;
    MarsPlayer* player = new MarsPlayer(options, native_tick);
    s_player_map[idx] = player;

    class Runnable : public MarsThreadEvent {
    public:
        Runnable(MarsPlayer* player) : player_(player) {
        }

        void Run() override {
            PlayerEventEmitter::OnEvent(player_->GetPlayerId(), EVENT_TYPE_THREAD_START, MN_THREAD_NAME);
            player_->OnSurfaceCreate(nullptr);
        }

    private:
        MarsPlayer* player_;
    };
    player->GetTicker()->Post(new Runnable(player));
}

void MarsPlayerManager::AddPlugin(int idx, void* ptr, const std::string& name) {
    std::lock_guard<std::recursive_mutex> lock(s_player_mutex);
    auto iter = s_player_map.find(idx);
    if (iter == s_player_map.end()) {
        MLOGE("player not exist %s", __FUNCTION__);\
        return;
    }

    MarsPlayer* player = iter->second;
    player->RegisterExtensionPlugin(name, ptr);
}

void MarsPlayerManager::SetRepeatCount(int idx, int repeat_count) {
    std::lock_guard<std::recursive_mutex> lock(s_player_mutex);
    auto iter = s_player_map.find(idx);
    if (iter == s_player_map.end()) {
        MLOGE("player not exist %s", __FUNCTION__);\
        return;
    }

    class Runnable : public MarsThreadEvent {
    public:
        Runnable(MarsPlayer* player, int repeat_count) : player_(player), repeat_count_(repeat_count) {
        }

        ~Runnable() {
        }

        void Run() override {
            player_->SetRepeatCount(repeat_count_);
        }

    private:
        int repeat_count_;
        MarsPlayer* player_;
    };

    MarsPlayer* player = iter->second;
    player->GetTicker()->Post(new Runnable(player, repeat_count));
}

void MarsPlayerManager::SetSceneData(int idx, SceneDataDecoder* scene_data) {
    std::lock_guard<std::recursive_mutex> lock(s_player_mutex);
    auto iter = s_player_map.find(idx);
    if (iter == s_player_map.end()) {
        MLOGE("player not exist %s", __FUNCTION__);
        delete scene_data;
        return;
    }

    class Runnable : public MarsThreadEvent {
    public:
        Runnable(MarsPlayer* player, SceneDataDecoder* scene_data) : player_(player), scene_data_(scene_data) {
        }

        ~Runnable() {
            if (scene_data_) {
                delete scene_data_;
                scene_data_ = nullptr;
            }
        }

        void Run() override {
            player_->SetSceneData(scene_data_);
            scene_data_ = nullptr;
        }

    private:
        SceneDataDecoder* scene_data_;
        MarsPlayer* player_;
    };

    MarsPlayer* player = iter->second;
    player->GetTicker()->Post(new Runnable(player, scene_data));
}

void MarsPlayerManager::OnSurfaceCreate(int idx, void* surface) {
    std::lock_guard<std::recursive_mutex> lock(s_player_mutex);
    auto iter = s_player_map.find(idx);
    if (iter == s_player_map.end()) {
        MLOGE("player not exist %s", __FUNCTION__);
        return;
    }

    class Runnable : public MarsThreadEvent {
    public:
        Runnable(MarsPlayer* player, void* surface) : player_(player), surface_(surface) {
        }

        ~Runnable() {
        }

        void Run() override {
            player_->OnSurfaceCreate(surface_);
        }

    private:
        MarsPlayer* player_;
        void* surface_;
    };

    MarsPlayer* player = iter->second;
    player->GetTicker()->Post(new Runnable(player, surface));
}

void MarsPlayerManager::OnSurfaceResize(int idx, int width, int height) {
    std::lock_guard<std::recursive_mutex> lock(s_player_mutex);
    auto iter = s_player_map.find(idx);
    if (iter == s_player_map.end()) {
        MLOGE("player not exist %s", __FUNCTION__);
        return;
    }
    
    class Runnable : public MarsThreadEvent {
    public:
        Runnable(MarsPlayer* player, int width, int height) : player_(player), width_(width), height_(height) {
            
        }

        ~Runnable() {
        }

        void Run() override {
            player_->OnSurfaceResize(width_, height_);
        }

    private:
        MarsPlayer* player_;
        int width_;
        int height_;
    };
    
    MarsPlayer* player = iter->second;
    player->GetTicker()->Post(new Runnable(player, width, height));
}

void MarsPlayerManager::OnSurfaceDestroy(int idx) {
    std::lock_guard<std::recursive_mutex> lock(s_player_mutex);
    auto iter = s_player_map.find(idx);
    if (iter == s_player_map.end()) {
        MLOGE("player not exist %s", __FUNCTION__);
        return;
    }

    class Runnable : public MarsThreadEvent {
    public:
        Runnable(MarsPlayer* player) : player_(player) {
        }

        ~Runnable() {
        }

        void Run() override {
            player_->OnSurfaceCreate(nullptr);
        }

    private:
        MarsPlayer* player_;
    };

    MarsPlayer* player = iter->second;
    player->GetTicker()->Post(new Runnable(player));
}

void MarsPlayerManager::PlayMars(int idx, const std::string& token, bool need_lock) {
    std::lock_guard<std::recursive_mutex> lock(s_player_mutex);
    auto iter = s_player_map.find(idx);

    if (iter == s_player_map.end()) {
        MLOGE("player not exist %s", __FUNCTION__);
        return;
    }

    MLOGE("MarsPlayerManager::PlayMars %d, lock: %d", idx, need_lock);
    class Runnable : public MarsThreadEvent {
    public:
        Runnable(MarsPlayer* player, const std::string& token, bool need_lock) : player_(player), token_(token), need_lock_(need_lock) {
        }

        ~Runnable() {
        }

        void Run() override {
            player_->setPlayToken(token_);
            if (need_lock_) {
                std::lock_guard<std::mutex> lock(s_start_mutex);
                player_->Start();
            } else {
                player_->Start();
            }
        }

    private:
        MarsPlayer* player_;
        bool need_lock_ = false;
        std::string token_;
    };

    MarsPlayer* player = iter->second;
    player->GetTicker()->Post(new Runnable(player, token, need_lock));
}

void MarsPlayerManager::PlayMarsFrameControl(int idx, int from_frame, int to_frame, const std::string& token, bool need_lock) {
    std::lock_guard<std::recursive_mutex> lock(s_player_mutex);
    auto iter = s_player_map.find(idx);

    if (iter == s_player_map.end()) {
        MLOGE("MarsPlayerJNI::PlayMarsFrameControl no player");
        return;
    }

    MLOGE("MarsPlayerJNI::PlayMarsFrameControl %d %d %d, lock: %d", idx, from_frame, to_frame, need_lock);
    class Runnable : public MarsThreadEvent {
    public:
        Runnable(MarsPlayer* player, int from_frame, int to_frame, const std::string& token, bool need_lock) :
                player_(player), from_frame_(from_frame), to_frame_(to_frame), token_(token),
                need_lock_(need_lock) {
        }

        ~Runnable() {
        }

        void Run() override {
            player_->setPlayToken(token_);
            player_->SetFrameControl(from_frame_, to_frame_, 33);
            if (!player_->IsStarted()) {
                if (need_lock_) {
                    std::lock_guard<std::mutex> lock(s_start_mutex);
                    player_->Start();
                } else {
                    player_->Start();
                }
            } else {
                player_->Restart(from_frame_ * 33);
            }
        }

    private:
        MarsPlayer* player_;
        int from_frame_;
        int to_frame_;
        bool need_lock_ = false;
        std::string token_;
    };

    MarsPlayer* player = iter->second;
    player->GetTicker()->Post(new Runnable(player, from_frame, to_frame, token, need_lock));
}

void MarsPlayerManager::PlayMarsFrameTick(int idx, int ms) {
    std::lock_guard<std::recursive_mutex> lock(s_player_mutex);
    auto iter = s_player_map.find(idx);
    if (iter == s_player_map.end()) {
        MLOGE("player not exist %s", __FUNCTION__);
        return;
    }

    class Runnable : public MarsThreadEvent {
    public:
        Runnable(MarsPlayer* player, int ms) : player_(player), ms_(ms) {
        }

        ~Runnable() {
        }

        void Run() override {
            player_->Tick(ms_);
        }

        bool IsTickEvent() override {
            return true;
        }

    private:
        MarsPlayer* player_;
        int ms_;
    };

    MarsPlayer* player = iter->second;
    player->GetTicker()->Post(new Runnable(player, ms));
}

void MarsPlayerManager::StopMars(int idx) {
    std::lock_guard<std::recursive_mutex> lock(s_player_mutex);
    auto iter = s_player_map.find(idx);
    if (iter == s_player_map.end()) {
        MLOGE("player not exist %s", __FUNCTION__);
        return;
    }
    
    class Runnable : public MarsThreadEvent {
    public:
        Runnable(MarsPlayer* player) : player_(player) {
        }

        ~Runnable() {
        }

        void Run() override {
            player_->Stop();
        }

    private:
        MarsPlayer* player_;
    };
    
    MarsPlayer* player = iter->second;
    player->GetTicker()->Post(new Runnable(player));
}

void MarsPlayerManager::RestartMars(int idx, int start_time) {
    std::lock_guard<std::recursive_mutex> lock(s_player_mutex);
    auto iter = s_player_map.find(idx);
    if (iter == s_player_map.end()) {
        MLOGE("player not exist %s", __FUNCTION__);
        return;
    }
    
    class Runnable : public MarsThreadEvent {
    public:
        Runnable(MarsPlayer* player, int start_time) : player_(player), start_time_(start_time) {
        }

        ~Runnable() {
        }

        void Run() override {
            player_->Restart(start_time_);
        }

    private:
        MarsPlayer* player_;
        int start_time_;
    };
    
    MarsPlayer* player = iter->second;
    player->GetTicker()->Post(new Runnable(player, start_time));
}

void MarsPlayerManager::DestroyMars(int idx) {
    MLOGD("MarsPlayerManager::DestroyMars %d", idx);
    std::lock_guard<std::recursive_mutex> lock(s_player_mutex);
    auto iter = s_player_map.find(idx);
    if (iter == s_player_map.end()) {
        MLOGE("MarsPlayerJNI::DestroyMars no player");
        return;
    }
    
    MarsPlayer* player = iter->second;
    s_player_map.erase(iter);
    
    class Runnable : public MarsThreadEvent {
    public:
        Runnable(MarsPlayer* player) : player_(player) {
        }

        ~Runnable() {
        }

        void Run() override {
            int player_id = player_->GetPlayerId();
            {
                std::lock_guard<std::mutex> destroy_lock(s_destroy_mutex);
                delete player_;
            }
            PlayerEventEmitter::OnEvent(player_id, EVENT_TYPE_THREAD_END, MN_THREAD_NAME);
        }

        bool IsDestroyEvent() override { return true; }

    private:
        MarsPlayer* player_;
    };
    {
        // 解决极端情况下，Post发出的事件立即执行，早于player->Resume()执行完，引起异常
        std::lock_guard<std::mutex> destroy_lock(s_destroy_mutex);
        player->GetTicker()->Post(new Runnable(player));
        player->Resume();
    }
}

void MarsPlayerManager::OnEvent(int idx, int type) {
    std::lock_guard<std::recursive_mutex> lock(s_player_mutex);
    auto iter = s_player_map.find(idx);
    if (iter == s_player_map.end()) {
        MLOGE("player not exist %s", __FUNCTION__);
        return;
    }

    MarsPlayer* player = iter->second;
    if (type == 1) {
        player->Pause();
    } else if (type == 2) {
        player->Resume();
    }
}

void MarsPlayerManager::PlayMarsToFrame(int idx, int frame_idx, int frame_duration) {
    std::lock_guard<std::recursive_mutex> lock(s_player_mutex);
    auto iter = s_player_map.find(idx);
    if (iter == s_player_map.end()) {
        MLOGE("MarsPlayerJNI::PlayMars no player:%d" , idx);
        return;
    }

    class Runnable : public MarsThreadEvent {
    public:
        Runnable(MarsPlayer* player, int frame_idx, int frame_duration) : player_(player), frame_idx_(frame_idx), frame_duration_(frame_duration) {
        }

        ~Runnable() {
        }

        void Run() override {
            player_->TickToFrame(frame_idx_, frame_duration_);
        }

        bool IsTickEvent() override {
            return true;
        }

    private:
        MarsPlayer* player_;
        int frame_idx_;
        int frame_duration_;
    };

    MarsPlayer* player = iter->second;
    player->GetTicker()->Post(new Runnable(player, frame_idx, frame_duration));
}

void* MarsPlayerManager::CreateVideoContextAndPrepare(
        int idx, const std::string& file_path, const std::string& key, bool transparent, bool hard_decoder,
        const VideoPrepareCallback& callback) {
    std::lock_guard<std::recursive_mutex> lock(s_player_mutex);
    auto iter = s_player_map.find(idx);
    if (iter == s_player_map.end()) {
        MLOGE("player not exist %s", __FUNCTION__);
        return nullptr;
    }

    class Runnable : public MarsThreadEvent {
    public:
        Runnable(VideoContext* vc, bool transparent, bool hard_decoder, const VideoPrepareCallback& callback)
                : vc_(vc), transparent_(transparent), hard_decoder_(hard_decoder), callback_(callback) {
        }

        ~Runnable() {
        }

        void Run() override {
            vc_->Prepare(transparent_, hard_decoder_, callback_);
        }

        bool IsTickEvent() override {
            return true;
        }

    private:
        VideoContext* vc_;
        bool transparent_;
        bool hard_decoder_;
        VideoPrepareCallback callback_;
    };

    MarsPlayer* player = iter->second;
    VideoContext* vc = player->CreateVideoContext(file_path, key);
    player->GetTicker()->Post(new Runnable(vc, transparent, hard_decoder, callback));
    return vc;
}

bool MarsPlayerManager::UpdateMarsImage(int idx, int templateIdx, ImageRawData* raw_data) {
    std::lock_guard<std::recursive_mutex> lock(s_player_mutex);
    auto iter = s_player_map.find(idx);
    if (iter == s_player_map.end()) {
        MLOGE("player not exist %s", __FUNCTION__);
        return false;
    }
    
    class Runnable : public MarsThreadEvent {
    public:
        Runnable(MarsPlayer* player, int templateIdx, ImageRawData* raw_data)
            : player_(player), templateIdx_(templateIdx), raw_data_(raw_data) {
        }
        
        ~Runnable () {
            delete raw_data_;
        }
        
        void Run() override {
            player_->updateTexture(templateIdx_, raw_data_->width, raw_data_->height, raw_data_->data);
        }

    private:
        MarsPlayer* player_;
        int templateIdx_;
        ImageRawData* raw_data_;
    };
    
    MarsPlayer* player = iter->second;
    player->GetTicker()->Post(new Runnable(player, templateIdx, raw_data));
    return true;
}

}
