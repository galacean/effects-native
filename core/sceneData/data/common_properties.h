#ifndef MN_SCENEDATA_DATA_COMMON_PROPERTIES_H_
#define MN_SCENEDATA_DATA_COMMON_PROPERTIES_H_

#include "sceneData/mars_data_base.h"
#include "sprite_mesh_geometry.hpp"
#include "util/util.hpp"

namespace mn {

struct ItemRendererOptionsData {
    MInt* render_mode = nullptr;
    MInt* blending = nullptr;
    MInt* side = nullptr;
    MBool* occlusion = nullptr;
    MBool* transparent_occlusion = nullptr;
    MInt* texture = nullptr;
    MInt* mask_mode = nullptr;
    MInt* particle_origin = nullptr;
    MFloat* order = nullptr;
    MInt* shape_id = nullptr;
    SpriteMeshGeometry* shape = nullptr;
    MInt* mask = nullptr;
    Vec2* anchor = nullptr;

    ~ItemRendererOptionsData() {
        MN_SAFE_DELETE(render_mode);
        MN_SAFE_DELETE(blending);
        MN_SAFE_DELETE(side);
        MN_SAFE_DELETE(occlusion);
        MN_SAFE_DELETE(transparent_occlusion);
        MN_SAFE_DELETE(texture);
        MN_SAFE_DELETE(mask_mode);
        MN_SAFE_DELETE(particle_origin);
        MN_SAFE_DELETE(order);
        MN_SAFE_DELETE(shape_id);
        MN_SAFE_DELETE(shape);
        MN_SAFE_DELETE(mask);
        MN_SAFE_DELETE(anchor);
    }
};

struct ItemSizeOverLifetimeData {
    MBool* separate_axes = nullptr;
    MNumberOrValue* size = nullptr;
    MNumberOrValue* x = nullptr;
    MNumberOrValue* y = nullptr;
    MNumberOrValue* z = nullptr;

    ~ItemSizeOverLifetimeData() {
        MN_SAFE_DELETE(separate_axes);
        MN_SAFE_DELETE(size);
        MN_SAFE_DELETE(x);
        MN_SAFE_DELETE(y);
        MN_SAFE_DELETE(z);
    }
};

struct ItemRotateOverLifetimeData {
    MBool* separate_axes = nullptr;
    MBool* as_rotation = nullptr;
    MNumberOrValue* x = nullptr;
    MNumberOrValue* y = nullptr;
    MNumberOrValue* z = nullptr;

    ~ItemRotateOverLifetimeData() {
        MN_SAFE_DELETE(separate_axes);
        MN_SAFE_DELETE(as_rotation);
        MN_SAFE_DELETE(x);
        MN_SAFE_DELETE(y);
        MN_SAFE_DELETE(z);
    }
};

struct ParticleContentEmitterTransformData {
    MValue* path_value = nullptr;
    float* path_arr = nullptr;

    ~ParticleContentEmitterTransformData() {
        MN_SAFE_DELETE(path_value);
    }
};

struct ItemPositionOverLifetimeData : public ParticleContentEmitterTransformData {
    MBool* as_movement = nullptr;
    MNumberOrValue* linear_x = nullptr;
    MNumberOrValue* linear_y = nullptr;
    MNumberOrValue* linear_z = nullptr;
    MBool* as_rotation = nullptr;
    MNumberOrValue* orbital_x = nullptr;
    MNumberOrValue* orbital_y = nullptr;
    MNumberOrValue* orbital_z = nullptr;
    float* orbital_center = nullptr;
    float* direction = nullptr;
    MNumberOrValue* start_speed = nullptr;
    MNumberOrValue* speed_over_lifetime = nullptr;
    float* gravity = nullptr;
    MNumberOrValue* gravity_modifier = nullptr;
    MBool* force_target = nullptr;
    float* target = nullptr;
    MNumberOrValue* force_curve = nullptr;
    bool useHighpFloatOnIOS = false;

    ~ItemPositionOverLifetimeData() {
        MN_SAFE_DELETE(as_movement);
        MN_SAFE_DELETE(linear_x);
        MN_SAFE_DELETE(linear_y);
        MN_SAFE_DELETE(linear_z);
        MN_SAFE_DELETE(as_rotation);
        MN_SAFE_DELETE(orbital_x);
        MN_SAFE_DELETE(orbital_y);
        MN_SAFE_DELETE(orbital_z);
        MN_SAFE_DELETE(start_speed);
        MN_SAFE_DELETE(speed_over_lifetime);
        MN_SAFE_DELETE(gravity_modifier);
        MN_SAFE_DELETE(force_target);
        MN_SAFE_DELETE(force_curve);
    }
};

struct ItemColorOverLifetimeData {
    MNumberOrValue* opacity = nullptr;
    MNumberOrValue* color = nullptr;

    ~ItemColorOverLifetimeData() {
        MN_SAFE_DELETE(opacity);
        MN_SAFE_DELETE(color);
    }
};

struct ItemTextureSheetAnimationData {
    MInt* row = nullptr;
    MInt* col = nullptr;
    MInt* total = nullptr;
    MBool* animate = nullptr;
    MNumberOrValue* cycles = nullptr;
    MNumberOrValue* animation_delay = nullptr;
    MNumberOrValue* animation_duration = nullptr;

    ~ItemTextureSheetAnimationData() {
        MN_SAFE_DELETE(row);
        MN_SAFE_DELETE(col);
        MN_SAFE_DELETE(total);
        MN_SAFE_DELETE(animate);
        MN_SAFE_DELETE(cycles);
        MN_SAFE_DELETE(animation_delay);
        MN_SAFE_DELETE(animation_duration);
    }
};

struct ItemSplitsData {
    std::vector<std::vector<float>> splits;
};

struct ItemFilterData {
    std::string name;
};

struct GaussianFilterData : ItemFilterData {
    MInt* radius = nullptr;

    GaussianFilterData() {
        name = "gaussian";
    }
    
    ~GaussianFilterData() {
        MN_SAFE_DELETE(radius);
    }
};

struct DelayFilterData : ItemFilterData {
    MNumberOrValue* strength = nullptr;
    
    DelayFilterData() {
        name = "delay";
    }
    
    ~DelayFilterData() {
        MN_SAFE_DELETE(strength);
    }
};

struct DistortionFilterData : ItemFilterData {
    float* center = nullptr;
    float* direction = nullptr;
    MNumberOrValue* strength = nullptr;
    MNumberOrValue* period = nullptr;
    MNumberOrValue* wave_movement = nullptr;
    
    DistortionFilterData() {
        name = "distortion";
    }
    
    ~DistortionFilterData() {
        MN_SAFE_DELETE(center);
        MN_SAFE_DELETE(direction);
        MN_SAFE_DELETE(strength);
        MN_SAFE_DELETE(period);
        MN_SAFE_DELETE(wave_movement);
    }
};

struct BloomFilterData : ItemFilterData {
    MInt* radius = nullptr;
    float* color_weight = nullptr;
    MNumberOrValue* threshold = nullptr;
    MNumberOrValue* bloom_add_on = nullptr;
    MNumberOrValue* color_add_on = nullptr;
    
    BloomFilterData() {
        name = "bloom";
    }

    ~BloomFilterData() {
        MN_SAFE_DELETE(radius);
        MN_SAFE_DELETE(color_weight);
        MN_SAFE_DELETE(threshold);
        MN_SAFE_DELETE(bloom_add_on);
        MN_SAFE_DELETE(color_add_on);
    }
};

}

#endif
