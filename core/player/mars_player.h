#ifndef MN_PLAYER_MARS_PLAYER_H_
#define MN_PLAYER_MARS_PLAYER_H_

#include <vector>
#include <map>
#include "composition.h"
#include "ticker.h"
#include "plugin_system.h"
#include "sceneData/scene_data_decoder.h"
#include "util/config.h"
#include "video/video_image_data.hpp"

namespace mn {

class MarsSurface;
class MarsRenderer;
class RenderFrame;

class PlayerEventEmitter {
public:
    static void OnEvent(int player_id, int type, std::string msg);
};

class PlayerEventHandler {
public:
    virtual void OnEvent(int type, const std::string& msg) = 0;
};

struct MarsPlayerConstructor {
    MarsSurface* surface = nullptr;
    bool manual_render = false;
    bool auto_tick = true;
    int fps = 30;
    int idx = 0;

    bool lock_tick_event = false;
    MarsConfig config;
};
    
class MarsPlayer : public TickerProtocal {
public:
    // todo：native_tick区分c++ tick还是系统tick，90版本下掉
    MarsPlayer(const MarsPlayerConstructor& options, bool native_tick);

    virtual ~MarsPlayer();

    void SetRepeatCount(int repeat_count);

    void SetSceneData(SceneDataDecoder* scene_data);

    void OnSurfaceCreate(void* surface);
    
    void OnSurfaceResize(int width, int height);

    void Start();
    
    void Restart(int start_time);

    void Pause();

    void Resume();
    
    void Stop();

    void Tick(int dt) override; // 单位：ms
    
    void TickToFrame(int frame_idx, int frame_duration); // 单位：ms
    
    void TickInternal(int dt, bool fix_time);

    Ticker* GetTicker() const {
        return ticker_;
    }

    bool IsSupportCompressedTexture() const;

    bool CreateTextureContainers();
    
    VideoContext* CreateVideoContext(const std::string& file_path, const std::string& key);

    int GetPlayerId() const {
        return player_id_;
    }

    void RegisterEventHandler(PlayerEventHandler* handler);

    bool RegisterNativePlugin(const std::string& name, MarsNativePluginBuilder* native_plugin);

    void RegisterExtensionPlugin(const std::string& name, void* ptr);

// c++帧控制 begin
    void SetFrameControl(int from_frame, int to_frame, int frame_duration); // 单位：ms
    
    bool IsStarted() const {
        return start_;
    }
// c++帧控制 end
    
    void setPlayToken(const std::string& token) {
        play_token_ = token;
    }
    
    void updateTexture(int idx, int w, int h, uint8_t* data);

protected:
    void Init();
    
    Composition* InitializeComposition();

    void EmitEvent(int type, const std::string& msg);

protected:
#ifdef UNITTEST
public:
#endif
    int player_id_ = 0;

    MarsRenderer* renderer_ = nullptr;
    MarsSurface* surface_ = nullptr;

    std::mutex ticker_mutex_;
    Ticker* ticker_ = nullptr;
        
    SceneDataDecoder* scene_data_ = nullptr;

    std::map<int, std::shared_ptr<MarsTexture>> textures_;
    std::vector<std::shared_ptr<VideoContext>> videos_;
    
    std::vector<Composition*> compositions_;

    PluginSystem* plugin_system_ = nullptr;

    RenderFrame* clear_frame_ = nullptr;
    
    bool inited_ = false;
    
    bool start_ = false;

    int repeat_count_ = -100;
    
    bool glerror_uploaded_ = false;

    PlayerEventHandler* event_handler_ref_ = nullptr;
    
    int render_level_ = 0;
    
    int current_time_ = 0;

// c++帧控制 begin
    bool native_tick_ = false;

    bool stoped_by_player_ = false;

    int start_time_ = -1;

    int end_time_ = -1;
    
    int duration_ms_ = 0;
// c++帧控制 end
    bool fix_tick_time_ = true; // 是否限制每帧的最大dt
    
    std::string play_token_;
};

}

#endif
