#ifndef MN_SCENEDATA_DATA_COMPOSITION_H_
#define MN_SCENEDATA_DATA_COMPOSITION_H_

#include "item.h"

namespace mn {

struct CameraData {
    float near;
    float far;
    float fov;
    float* position = nullptr;
    float* rotation = nullptr;
    MInt* clip_mode = nullptr;

    ~CameraData() {
        MN_SAFE_DELETE(clip_mode);
    }
};

struct CompositionData : public ItemData {
    CameraData* camera = nullptr;
    std::vector<ItemData*> items;
    float* preview_size = nullptr;
    std::string fallback_image;

    ~CompositionData() {
        MN_SAFE_DELETE(camera);
        for (auto item : items) {
            MN_SAFE_DELETE(item);
        }
    }
};

}

#endif
