#ifndef MN_SCENEDATA_DATA_CONTENTS_H_
#define MN_SCENEDATA_DATA_CONTENTS_H_

#include "common_properties.h"

namespace mn {

struct ContentDataBase {
    ItemRendererOptionsData* renderer = nullptr;

    virtual ~ContentDataBase() {
        MN_SAFE_DELETE(renderer);
    }
};

struct SpriteContentOptionsData {
    MValue* start_color = nullptr;

    ~SpriteContentOptionsData() {
        MN_SAFE_DELETE(start_color);
    }
};

struct SpriteContentInteractionData {
    MInt* behavior = nullptr;

    ~SpriteContentInteractionData() {
        MN_SAFE_DELETE(behavior);
    }
};

struct SpriteContentData : public ContentDataBase {
    ItemSplitsData* splits = nullptr;
    SpriteContentOptionsData* options = nullptr;
    ItemSizeOverLifetimeData* size_over_lifetime = nullptr;
    ItemRotateOverLifetimeData* rotation_over_lifetime = nullptr;
    ItemPositionOverLifetimeData* position_over_lifetime = nullptr;
    ItemColorOverLifetimeData* color_over_lifetime = nullptr;
    ItemTextureSheetAnimationData* texture_sheet_animation = nullptr;
    SpriteContentInteractionData* interaction = nullptr;
    ItemFilterData* filter_data = nullptr;

    ~SpriteContentData() {
        MN_SAFE_DELETE(splits);
        MN_SAFE_DELETE(options);
        MN_SAFE_DELETE(size_over_lifetime);
        MN_SAFE_DELETE(rotation_over_lifetime);
        MN_SAFE_DELETE(position_over_lifetime);
        MN_SAFE_DELETE(color_over_lifetime);
        MN_SAFE_DELETE(texture_sheet_animation);
        MN_SAFE_DELETE(interaction);
        MN_SAFE_DELETE(filter_data);
    }
};

struct ParticleContentOptionsData {
    MInt* max_count = nullptr;
    MNumberOrValue* start_lifetime = nullptr;
    MNumberOrValue* start_delay = nullptr;
    MValue* start_color = nullptr;
    MNumberOrValue* start_size = nullptr;
    MNumberOrValue* size_aspect = nullptr;
    MBool* start_3d_size = nullptr;
    MNumberOrValue* start_size_x = nullptr;
    MNumberOrValue* start_size_y = nullptr;
    MNumberOrValue* start_rotation_x = nullptr;
    MNumberOrValue* start_rotation_y = nullptr;
    MNumberOrValue* start_rotation_z = nullptr;

    ~ParticleContentOptionsData() {
        MN_SAFE_DELETE(max_count);
        MN_SAFE_DELETE(start_lifetime);
        MN_SAFE_DELETE(start_delay);
        MN_SAFE_DELETE(start_color);
        MN_SAFE_DELETE(start_size);
        MN_SAFE_DELETE(size_aspect);
        MN_SAFE_DELETE(start_3d_size);
        MN_SAFE_DELETE(start_size_x);
        MN_SAFE_DELETE(start_size_y);
        MN_SAFE_DELETE(start_rotation_x);
        MN_SAFE_DELETE(start_rotation_y);
        MN_SAFE_DELETE(start_rotation_z);
    }
};

struct ParticleTextureShapeDetailData {
    float width = 0;
    float height = 0;
    float block[2] = {0, 0};
    float* anchors = nullptr;

    ~ParticleTextureShapeDetailData() {
        MN_SAFE_DELETE_ARR(anchors);
    }
};

struct ParticleContentShapeData {
    const char* shape = nullptr;
    MFloat* angle = nullptr;
    MFloat* radius = nullptr;
    MFloat* width = nullptr;
    MFloat* height = nullptr;
    MNumberOrValue* turbulence_x = nullptr;
    MNumberOrValue* turbulence_y = nullptr;
    MNumberOrValue* turbulence_z = nullptr;
    MFloat* arc = nullptr;
    MInt* arc_mode = nullptr;
    MBool* align_speed_direction = nullptr;
    float* up_direction = nullptr;
    MFloat* random = nullptr;
    ParticleTextureShapeDetailData* detail = nullptr;

    ~ParticleContentShapeData() {
        MN_SAFE_DELETE(angle);
        MN_SAFE_DELETE(radius);
        MN_SAFE_DELETE(width);
        MN_SAFE_DELETE(height);
        MN_SAFE_DELETE(turbulence_x);
        MN_SAFE_DELETE(turbulence_y);
        MN_SAFE_DELETE(turbulence_z);
        MN_SAFE_DELETE(arc);
        MN_SAFE_DELETE(arc_mode);
        MN_SAFE_DELETE(align_speed_direction);
        MN_SAFE_DELETE(random);
        MN_SAFE_DELETE(detail);
    }
};

struct ParticleContentEmissionData {
    MNumberOrValue* rate_over_time = nullptr;
    std::vector<float> burst_offsets;
    std::vector<float> bursts;

    ~ParticleContentEmissionData() {
        MN_SAFE_DELETE(rate_over_time);
    }
};

struct ParticleContentInteractionData {
    MInt* behavior = nullptr;
    MBool* multiple = nullptr;
    MFloat* radius = nullptr;

    ~ParticleContentInteractionData() {
        MN_SAFE_DELETE(behavior);
        MN_SAFE_DELETE(multiple);
        MN_SAFE_DELETE(radius);
    }
};

struct ParticleContentData : public ContentDataBase {
    ItemSplitsData* splits = nullptr;
    ParticleContentOptionsData* options = nullptr;
    ParticleContentShapeData* shape = nullptr;
    ParticleContentEmissionData* emission = nullptr;
    ItemSizeOverLifetimeData* size_over_lifetime = nullptr;
    ParticleContentEmitterTransformData* emitter_transform = nullptr;
    ItemPositionOverLifetimeData* position_over_lifetime = nullptr;
    ItemRotateOverLifetimeData* rotation_over_lifetime = nullptr;
    ItemColorOverLifetimeData* color_over_lifetime = nullptr;
    ItemTextureSheetAnimationData* texture_sheet_animation = nullptr;
    ParticleContentInteractionData* interaction = nullptr;

    ~ParticleContentData() {
        MN_SAFE_DELETE(splits);
        MN_SAFE_DELETE(options);
        MN_SAFE_DELETE(shape);
        MN_SAFE_DELETE(emission);
        MN_SAFE_DELETE(size_over_lifetime);
        MN_SAFE_DELETE(emitter_transform);
        MN_SAFE_DELETE(position_over_lifetime);
        MN_SAFE_DELETE(rotation_over_lifetime);
        MN_SAFE_DELETE(color_over_lifetime);
        MN_SAFE_DELETE(texture_sheet_animation);
        MN_SAFE_DELETE(interaction);
    }
};

struct InteractContentOptionsData {
    int type;
};

struct InteractContentData : public ContentDataBase {
    InteractContentOptionsData* options = nullptr;

    ~InteractContentData() {
        MN_SAFE_DELETE(options);
    }
};

struct PluginContentOptionsData {
    int type;
};

struct PluginARContentOptions2DItemData {
    std::string item_id;
    int track_index;
    int track_start;
    int track_end;
};

struct PluginARContentOptionsData : public PluginContentOptionsData {
    std::string track_type;
    
    PluginARContentOptions2DItemData* items = nullptr;
    
    int items_count = 0;
    
    PluginARContentOptionsData() {
        type = 1;
    }
    
    ~PluginARContentOptionsData() {
        if (items) {
            delete[] items;
        }
    }
};

struct PluginContentData : public ContentDataBase {
    PluginContentOptionsData* options = nullptr;
    
    ~PluginContentData() {
        MN_SAFE_DELETE(options);
    }
};

}

#endif
