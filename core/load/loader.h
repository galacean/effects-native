#ifndef MN_LOAD_LOADER_H_
#define MN_LOAD_LOADER_H_

#include "util/constant.hpp"
#include "sceneData/scene_data_decoder.h"

namespace mn {

struct VFXCompositionItem {
    END_BEHAVIOR_TYPE end_behavior;
    bool looping;
    CameraData* camera = nullptr;
};

struct GetVFXItemOptions {
    int mask = 0;
    int base_render_order = 0;
    int render_level = 0;
};

class Loader {
public:
    static VFXCompositionItem* GetVFXItem(SceneDataDecoder* scene, const GetVFXItemOptions& options);
};

}

#endif
