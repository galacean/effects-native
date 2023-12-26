#ifndef MN_PLUGIN_TYPES_H_
#define MN_PLUGIN_TYPES_H_

#include <string>
#include "sceneData/scene_data_decoder.h"
#include "math/transform.h"
#include "util/util.hpp"
#include <unordered_map>
#include "math/vec2.hpp"
#include "math/vec3.hpp"
#include "math/vec4.hpp"
#include "math/math_util.hpp"
#include "math/value/value_getter.hpp"
#include "util/constant.hpp"
#include "util/color_utils.hpp"
#include "sceneData/data/sprite_mesh_geometry.hpp"

namespace mn {

class MarsRenderer;
class MarsTexture;
class Composition;

struct MeshRendererOptions {
    MarsRenderer* renderer = nullptr;
    std::shared_ptr<MarsTexture> empty_texture;
    std::string cache_prefix;
    Composition* composition = nullptr;
};

// todo: 当前calculateGroup的逻辑只能这么处理
struct ObjectLifeChecker {
};

class BaseContentItem {
public:
    
    std::shared_ptr<ObjectLifeChecker> life_checker_ = std::make_shared<ObjectLifeChecker>();
    
    void AddRef() {
        ++ref_;
    }
    
    void ReleaseRef() {
        --ref_;
        if (ref_ <= 0) {
            delete this;
        }
    }
    
protected:
    virtual ~BaseContentItem() {}
    
private:
    int ref_ = 1;
};

#define MN_SAFE_RELEASE_CONTENT(c) { if (c) { c->ReleaseRef(); c = nullptr; } }

struct ParticleStartOptions {
    ParticleStartOptions() {
    }
    
    ~ParticleStartOptions() {
        MN_SAFE_DELETE(start_speed);
        MN_SAFE_DELETE(start_life_time);
        MN_SAFE_DELETE(start_delay);
        MN_SAFE_DELETE(start_color);
        MN_SAFE_DELETE(start_rotation);
        MN_SAFE_DELETE(start_rotation_x);
        MN_SAFE_DELETE(start_rotation_y);
        MN_SAFE_DELETE(start_rotation_z);
        MN_SAFE_DELETE(start_size);
        MN_SAFE_DELETE(start_size_x);
        MN_SAFE_DELETE(start_size_y);
        MN_SAFE_DELETE(size_aspect);
        MN_SAFE_DELETE(gravity_modifier);
        for (auto ptr : turbulence) {
            MN_SAFE_DELETE(ptr);
        }
    }
    
    float duration = 0;
    bool looping = false;
    int max_count = 0;
    
    END_BEHAVIOR_TYPE end_behavior = END_BEHAVIOR_DESTROY;
    
    ValueGetter *start_speed = nullptr;
    ValueGetter *start_life_time = nullptr;
    ValueGetter* start_delay = nullptr;
    ValueGetter *start_color = nullptr;
    
    bool start_3D_rotation = false;
    ValueGetter *start_rotation_x = nullptr;
    ValueGetter *start_rotation_y = nullptr;
    ValueGetter *start_rotation_z = nullptr;
    ValueGetter *start_rotation = nullptr;
    
    bool start_3D_size = false;
    ValueGetter *start_size_x = nullptr;
    ValueGetter *start_size_y = nullptr;
    ValueGetter *start_size = nullptr;
    ValueGetter *size_aspect = nullptr;
    ValueGetter *gravity_modifier = nullptr;
    
    bool start_turbulence = false;
    std::vector<ValueGetter*> turbulence;
    
    // 引力方向
    Vec3 gravity;
};

struct RotationOverLifeTimeData {
    ~RotationOverLifeTimeData() {
        MN_SAFE_DELETE(rot_x)
        MN_SAFE_DELETE(rot_y)
        MN_SAFE_DELETE(rot_z)
    }
        
    bool as_rotation = false;
    ValueGetter *rot_x = nullptr;
    ValueGetter *rot_y = nullptr;
    ValueGetter *rot_z = nullptr;
};

struct LinearVelOverLifetime {
    bool as_movement = false;
    ValueGetter* x = nullptr;
    ValueGetter* y = nullptr;
    ValueGetter* z = nullptr;
    bool enabled = false;
    
    ~LinearVelOverLifetime() {
        MN_SAFE_DELETE(x);
        MN_SAFE_DELETE(y);
        MN_SAFE_DELETE(z);
    }
};

struct OrbitalVelOverLifetime {
    bool as_rotation = false;
    ValueGetter* x = nullptr;
    ValueGetter* y = nullptr;
    ValueGetter* z = nullptr;
    bool enabled = false;
    Vec3* center = nullptr;
    bool use_highp_float_on_ios = false;
    
    ~OrbitalVelOverLifetime() {
        MN_SAFE_DELETE(x);
        MN_SAFE_DELETE(y);
        MN_SAFE_DELETE(z);
        MN_SAFE_DELETE(center);
    }
};

struct SizeOverLifeTimeData {
    ~SizeOverLifeTimeData() {
        MN_SAFE_DELETE(size_x);
        MN_SAFE_DELETE(size_y);
    };
    
    bool separate_axes = false;
    ValueGetter* size_x = nullptr;
    ValueGetter* size_y = nullptr;
};

struct ColorOverLifeTimeData {
    ~ColorOverLifeTimeData() {
        MN_SAFE_DELETE(opacity);
        MN_SAFE_DELETE(separate_axes);
    }
        
    std::vector<ColorStop> color_steps;
    ValueGetter* opacity = nullptr;
    MBool* separate_axes = nullptr;
};

struct ForceTargetData {
    ValueGetter* curve = nullptr;
    Vec3 target;
    
    ~ForceTargetData() {
        MN_SAFE_DELETE(curve);
    }
};

struct TransfromOptions {
    ~TransfromOptions() {
        MN_SAFE_DELETE(path_lifetime_data);
    }
    
    void SetStartPosition(float x, float y, float z) {
        position.m[0] = x;
        position.m[1] = y;
        position.m[2] = z;
    }
    
    void SetStartRotation(float x, float y, float z) {
        rotation.m[0] = x;
        rotation.m[1] = y;
        rotation.m[2] = z;
    }
    
    Vec3 position;
    Vec3 rotation;
    ValueGetter *path_lifetime_data = nullptr;
};


// todo: shape option 继承实现多种类型
struct ParticleShapeOption {
    ParticleShapeOption() {
        radius = 0.0;
        arc = 0.0;
        arc_mode = ARC_MODE_RANDOM;
        up_direction.Set(0, 0, 1);
    };
    
    void SetArcMode(int value) {
        arc_mode = CreateArcModeType(value);
    }
    
    ~ParticleShapeOption(){}
    
    std::string shape;
    float radius;
    float arc;
    float width;
    float height;
    float angle = 0;
    ARC_MODE_TYPE arc_mode;
    bool align_speed_direction = false;
    Vec3 up_direction;
};

struct TextureSheetAnimation {
    TextureSheetAnimation() {
    }
    
    ~TextureSheetAnimation() {
        MN_SAFE_DELETE(animation_delay);
        MN_SAFE_DELETE(animation_duration);
        MN_SAFE_DELETE(cycles);
    }
    
    int col = 0;
    int row = 0;
    int total = 0;
    bool animate = false;
    bool blend = false;
    ValueGetter* animation_delay = nullptr;
    ValueGetter* animation_duration = nullptr;
    ValueGetter* cycles = nullptr;
};

struct ParticleMeshConstructor {
    ParticleMeshConstructor() {}

    ~ParticleMeshConstructor() {
        MN_SAFE_DELETE(size_over_lifetime);
        MN_SAFE_DELETE(force_target);
        MN_SAFE_DELETE(color_over_lifetime);
        MN_SAFE_DELETE(linear_vel_over_lifetime);
        MN_SAFE_DELETE(orbital_vel_over_lifetime);
        MN_SAFE_DELETE(rotation_over_lifetime);
        MN_SAFE_DELETE(speed_over_lifetime);
        
        MN_SAFE_DELETE(render_mode);
        MN_SAFE_DELETE(blending);
        MN_SAFE_DELETE(mask);
        MN_SAFE_DELETE(mask_mode);
        MN_SAFE_DELETE(side);
        MN_SAFE_DELETE(transparent_occlusion);
        MN_SAFE_DELETE(matrix);
        MN_SAFE_DELETE(gravity);
        MN_SAFE_DELETE(anchor);
    }
    
    ValueGetter* gravity_modifier_ref = nullptr;
    SizeOverLifeTimeData* size_over_lifetime = nullptr;
    ForceTargetData* force_target = nullptr;
    ColorOverLifeTimeData* color_over_lifetime = nullptr;
    LinearVelOverLifetime* linear_vel_over_lifetime = nullptr;
    OrbitalVelOverLifetime* orbital_vel_over_lifetime = nullptr;
    RotationOverLifeTimeData* rotation_over_lifetime = nullptr;
    ValueGetter* speed_over_lifetime = nullptr;
    
    MInt* render_mode = nullptr;
    MInt* blending = nullptr;
    MInt* mask = nullptr;
    MInt* mask_mode = nullptr;
    MInt* side = nullptr;
    MBool* transparent_occlusion = nullptr;
    Mat4* matrix = nullptr;
    TextureSheetAnimation* sprite_ref = nullptr;
    Vec3* gravity = nullptr;
    bool use_sprite = false;
    bool texture_flip = false;
    bool occlusion = false;
    std::shared_ptr<MarsTexture> diffuse;
    int list_index = 0;
    float duration = 0;
    int max_count = 0;
    std::string shader_cache_prefix;
    Vec2* anchor = nullptr;
    std::string name;
};

struct ParticleTransform {
    virtual ~ParticleTransform() {
        MN_SAFE_DELETE(position_);
        MN_SAFE_DELETE(rotation_);
        MN_SAFE_DELETE(path_);
    }

    Vec3* position_ = nullptr;
    Vec3* rotation_ = nullptr;
    ValueGetter* path_ = nullptr;
};

struct BurstGenerateOption {    
    bool valid = false;
    int index = 0;
    float total = 0.0;
    int count = 0;
    int cycle_index = 0;
};

struct BurstOption { 
    void GetGeneratorOption(BurstGenerateOption& opt, float time_passed, float life_time) {
        float dt = time_passed - this->time;
        if (dt > this->interval * this->_i && this->_cycles > 0) {
            this->_cycles--;
            this->_i++;
            
            if (MathUtil::Random(0, 1) <= probability) {
                opt.valid = true;
                opt.index = this->_i;
                opt.count = this->count->GetValue(life_time);
                opt.total = 1.0 / this->interval;
                opt.cycle_index = this->cycles - this->_cycles - 1;
            }
            
        }
    }
    
    void Reset() {
        this->_cycles = this->cycles;
        this->_i = 0;
    }
    
    ~BurstOption() {
        MN_SAFE_DELETE(count);
    }
    
    float time = 0.0;
    float interval = 1.0;
    ValueGetter* count = nullptr;
    int cycles = 0;
    float probability = 1.0;
    
    int _now = 0;
    int _i = 0;
    int _cycles = 0;
};

struct ParticleEmissionOptions {
    
    ParticleEmissionOptions() {
        rate_overtime = nullptr;
    }
    
    ~ParticleEmissionOptions() {
        if (rate_overtime) {
            delete rate_overtime;
        }
    }
    
    void SetBurstOffset(int index, Vec3 pos) {
        if (burst_offsets.find(index) == burst_offsets.end()) {
            std::vector<Vec3> pos_vec;
            pos_vec.push_back(pos);
            burst_offsets.insert({index, pos_vec});
            return ;
        }
        
        auto iter = burst_offsets.find(index);
        iter->second.push_back(pos);
        return ;
    }
    
    void GetBurstOffset(Vec3& pos, int index, int cycle_index) {
        if (burst_offsets.find(index) != burst_offsets.end()) {
            auto iter = burst_offsets.find(index);
            auto& pos_vec = iter->second;
            size_t len = pos_vec.size();
            if (cycle_index <= len - 1) {
                pos.Set(pos_vec[cycle_index]);
            }
        }
    }
    
    ValueGetter* rate_overtime;
    std::vector<std::shared_ptr<BurstOption>> bursts;
    std::unordered_map<int, std::vector<Vec3>> burst_offsets;
};

}

#endif
