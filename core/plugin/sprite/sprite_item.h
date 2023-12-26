#ifndef MN_PLUGIN_SPRITE_SPRITE_ITEM_H_
#define MN_PLUGIN_SPRITE_SPRITE_ITEM_H_

#include <string>
#include "math/transform.h"
#include "util/constant.hpp"
#include "util/util.hpp"
#include "math/value/value_getter.hpp"
#include "plugin/types.h"

// todo
#include "sceneData/mars_data_base.h"
#include "sceneData/scene_data_decoder.h"
#include "sceneData/data/sprite_mesh_geometry.hpp"


namespace mn {

class SpriteRenderData;
class SpriteTranslateTarget;
class MarsTexture;
class VFXItem;
class FilterDefine;
class CustomMorph;

struct SpriteItemOptions {
    float start_speed;
    Vec3 direction;
    float start_size = 0;
    float size_aspect = 0;
    Vec4 start_color;
    MFloat* start_rotation = nullptr;
    MFloat* start_3d_rotation = nullptr; // 图层未使用，粒子使用了，针对单个粒子的旋转， transform里面的rotation是针对发射器的；
    float duration;
    bool looping;
    Vec3 gravity;
    ValueGetter* gravity_modifier = nullptr;
    RENDER_LEVEL render_level;
    END_BEHAVIOR_TYPE end_behavior;

    ~SpriteItemOptions() {
        MN_SAFE_DELETE(start_rotation);
        MN_SAFE_DELETE(start_3d_rotation);
        MN_SAFE_DELETE(gravity_modifier);
    }
};

// todo
struct SpriteItemInteractionOptions {
};

struct SpriteItemRenderer {
    RENDER_MODE_TYPE render_mode;
    PARTICLE_ORIGIN_TYPE particle_origin;
    BLEND_MODE_TYPE blending;
    std::shared_ptr<MarsTexture> texture;
    bool occlusion;
    bool transparent_occlusion;
    SIDE_MODE_TYPE side;
    SpriteMeshGeometry* shape_ref = nullptr;
    int mask;
    MASK_MODE_TYPE mask_mode;
    float order;
    Vec2* anchor = nullptr;
    
    ~SpriteItemRenderer() {
        MN_SAFE_DELETE(anchor);
    }
};

struct SpriteItemTransform {
    Vec3 position;
    Vec3 rotation;
    ValueGetter* path = nullptr;

    ~SpriteItemTransform() {
        MN_SAFE_DELETE(path);
    }
};

// todo: 10.2.90 other filters;
struct SpriteItemFilter {
    std::string name;
    int radius;
};

struct SpriteItemRotationOverLifetime {
    bool as_rotation;
    bool separate_axes;
    ValueGetter* x = nullptr;
    ValueGetter* y = nullptr;
    ValueGetter* z = nullptr;
    bool enabled = false;

    ~SpriteItemRotationOverLifetime() {
        MN_SAFE_DELETE(x);
        MN_SAFE_DELETE(y);
        MN_SAFE_DELETE(z);
    }
};

struct SpriteItemPositionOverLifetime {
    MValue* path_value_ref = nullptr;
    Vec3* path_vec = nullptr;
    
    SpriteItemPositionOverLifetime(ItemPositionOverLifetimeData* data);
    
    ~SpriteItemPositionOverLifetime() {
        MN_SAFE_DELETE(path_vec);
    }
};

struct SpriteItemTextureSheet {
    int col = 0;
    int row = 0;
    int total = 0;
    bool animate = false;
};

// 缓存部分原始数据
struct SpriteItemCache {
    Vec3 start_size;
};

struct SpriteItemRenderInfo {
    SIDE_MODE_TYPE side;
    bool occlusion;
    BLEND_MODE_TYPE blending;
    std::string cache_prefix;
    int mask;
    MASK_MODE_TYPE mask_mode;
    std::string cache_id;
    std::shared_ptr<FilterDefine> filter;
    
    bool premultiply_alpha;
};

class SpriteItem : public BaseContentItem {
public:
    SpriteItem(SpriteContentData* props, MeshRendererOptions* opt, VFXItem* vfx_item);

    ~SpriteItem();

    virtual SpriteRenderData* GetRenderData(float time, bool init = false);

    void UpdateTime(float global_time);
    
    bool GetEnded();

    virtual void SetOption(SpriteContentData* props, VFXItem* vfx_item);

    void SetTransform(SpriteContentData* props);

    void SetRenderInfo(SpriteContentData* props, MeshRendererOptions* opt);

    void SetColorOverLifetime(SpriteContentData* props);

    void SetSizeOverLifetime(SpriteContentData* props);

    void SetVelOverLifetime(SpriteContentData* props);

    void SetRotOverLifetime(SpriteContentData* props);

    void SetSplits(SpriteContentData* props);
    
    void SetFilter(std::shared_ptr<FilterDefine> filter_define) {
        filter_define_ = filter_define;
    }
    
    std::shared_ptr<FilterDefine> GetFilter() {
        return filter_define_;
    }
    
    void SetCustomMorph(CustomMorph* morph);
    
    // anchor
    void FallbackAnchor();
    
    void GetStartSizeXY(float& x, float& y);
    void GetAnchorXY(float& x, float& y);
    
protected:
    virtual int InitTexture(int texture, MeshRendererOptions* options);

    virtual bool WillTranslate();

    Vec3* GetVelocity();
    
    std::shared_ptr<MarsTexture> InnerInitTexture(MInt* texture, MeshRendererOptions* opt);

public:
    SpriteItemOptions* options_ = nullptr;
    SpriteItemRenderer* renderer_ = nullptr;
    bool size_separate_axes_ = false;
    ValueGetter* speed_over_lifetime_ = nullptr;
    SpriteItemTransform* transform_ = nullptr;
    int list_index_ = 0;
    std::string parent_id_;
    bool reusable_ = false;
    bool hide_ = false;
    std::string name_;
    ValueGetter* gravity_modifier_ = nullptr;
    std::string cache_prefix_;
    SpriteMeshGeometry* inner_geo_data_ = nullptr;

    std::shared_ptr<Transform> inner_transform_;
    SpriteItemTextureSheet* texture_sheet_animation_ = nullptr;
    std::vector<std::vector<float>> splits_;
    float time_ = 0;
    float max_time_ = 0;
    ValueGetter* size_x_over_lifetime_ = nullptr;
    ValueGetter* size_y_over_lifetime_ = nullptr;
    ValueGetter* size_z_over_lifetime_ = nullptr;
    OrbitalVelOverLifetime* orbita_vel_over_lifetime_ = nullptr;
    SpriteItemRotationOverLifetime* rotation_over_lifetime_ = nullptr;
    std::vector<ColorStop> color_over_lifetime_;
    ValueGetter* opacity_over_lifetime_ = nullptr;
    LinearVelOverLifetime* linear_vel_over_lifetime_ = nullptr;
    SpriteItemRenderInfo* render_info_ = nullptr;
    float delay_ = 0;
    Vec3* velocity_ = nullptr;
    
    // 首页需求，临时实现，以后在mars编辑器支持
    CustomMorph* custom_motph_ = nullptr;
    float motph_lifetime_ = 0;
    
private:
    bool* will_translate_ = nullptr;
    
    friend class SpriteTranslateTarget;

    std::shared_ptr<FilterDefine> filter_define_;
    
    Vec2* anchor_ = nullptr;
    SpriteItemPositionOverLifetime* position_over_lifetime_ = nullptr;
    SpriteItemCache cache_;
};


}

#endif
