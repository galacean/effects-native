#ifndef MN_MARS_NATIVE_AR_TRACK_PLUGIN_H_
#define MN_MARS_NATIVE_AR_TRACK_PLUGIN_H_

#include "mars_native_player.h"
#include <vector>

namespace mn {

/**
 * AR追踪信息，如果追踪点数量为N，key_point_array的长度为2N：[x, y]
 * angle_array的长度为3：[pitch, yaw, roll]
 * mirror表示roll是否水平翻转
 * aspect为相机frame的宽高比
 */
struct MNARTrackInfo {
    std::vector<float> key_point_array;
    std::vector<float> angle_array;
    bool mirror = false;
    float aspect = 1;
};

class MarsNativeARTrackPlugin : public MarsNativePlugin {
public:
    MarsNativeARTrackPlugin() {}

    ~MarsNativeARTrackPlugin() {}

    /**
     * 获取跟踪信息的状态
     * @param dt 时间间隔，单位ms
     * @return MNARTrackInfo* 跟踪信息
     */
    virtual MNARTrackInfo* GetCurrentTrackInfo(int dt) = 0;
};

}

#endif
