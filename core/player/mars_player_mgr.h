#ifndef MN_PLAYER_MARS_PLAYER_MGR_H_
#define MN_PLAYER_MARS_PLAYER_MGR_H_

#include <stdint.h>
#include <string>
#include <mutex>
#include <map>
#include "platform/egl/mars_surface.hpp"
#include "mars_player.h"
#include "sceneData/scene_data_decoder.h"

namespace mn {

enum MarsPlayerEvent {
    Pause = 1,
    Resume
};

// c++唯一对外接口，一定在UI线程调用
class MarsPlayerManager {
public:
    static void CreateNew(int idx, MarsPlayerConstructor& options, bool native_tick);

    static void AddPlugin(int idx, void* ptr, const std::string& name);

    static void SetRepeatCount(int idx, int repeat_count);

    static void SetSceneData(int idx, SceneDataDecoder* scene_data);

    static void OnSurfaceCreate(int idx, void* surface = nullptr);
    
    static void OnSurfaceResize(int idx, int width, int height);

    static void OnSurfaceDestroy(int idx);

    /// 普通play，已废弃
    /// - Parameters:
    ///   - idx: player id
    ///   - token: 本次播放的token，标记对应的上层回调是哪一个
    ///   - need_lock: Android在play时锁一下，防止多线程同时创建shader，解决部分机型闪退
    static void PlayMars(int idx, const std::string& token, bool need_lock = false);

    /// play
    /// - Parameters:
    ///   - idx: player id
    ///   - from_frame: 播放起始帧
    ///   - to_frame: 播放结束帧
    ///   - token: 本次播放的token，标记对应的上层回调是哪一个
    ///   - need_lock: Android在play时锁一下，防止多线程同时创建shader，解决部分机型闪退
    static void PlayMarsFrameControl(int idx, int from_frame, int to_frame, const std::string& token, bool need_lock = false);

    static void StopMars(int idx);
    
    static void RestartMars(int idx, int start_time = 0);

    static void DestroyMars(int idx);

    static void PlayMarsFrameTick(int idx, int ms);
    
    static void PlayMarsToFrame(int idx, int frame_idx, int frame_duration);

    static void OnEvent(int idx, int type);
    
    static void* CreateVideoContextAndPrepare(
            int idx, const std::string& file_path, const std::string& key, bool transparent,
            bool hard_decoder, const VideoPrepareCallback& callback);
    
    static bool UpdateMarsImage(int idx, int templateIdx, ImageRawData* raw_data);

protected:
    static std::recursive_mutex s_player_mutex;
    static std::map<int, MarsPlayer*> s_player_map;
};

}

#endif
