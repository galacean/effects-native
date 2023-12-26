//
//  common.hpp
//
//  Created by Zongming Liu on 2022/9/23.
//

#ifndef common_hpp
#define common_hpp

#include <stdio.h>
#include "math/vec2.hpp"
#include "ri/render_base.hpp"
#include "ri/render/mars_geometry.hpp"
#include "plugin/tree/tree_item.hpp"
#include "util/constant.hpp"

namespace mn {

struct BaseTransform {
    BaseTransform() : position(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1) {}
    Vec3 position;
    Vec3 rotation;
    Vec3 scale;
};

struct BaseContentOptions {
    
};

struct MarsItemModelBase {
    virtual ~MarsItemModelBase() {};
    
    std::string id;
    int type;
    std::string parent_id;
    float delay;
    std::string name;
    float duration;
    int endBehavior;
    RENDER_LEVEL render_level;
    BaseTransform transform;
};

struct MarsSkinOptions {
    std::string name;
    std::vector<int> joints;
    int skeleton;
    uint8_t* inverse_bind_matrices; // dont free by MarsSkinOptions
    size_t matrices_byte_length;
};

enum MarsMaterialType {
    UNLit = 0,
    PBR = 1,
    SKYBOX = 2
};

enum MarsMaterialBlending {
    OPAQUE = 100,
    MASKED = 101,
    TRANSLUCENT = 102,
    ADDITIVE = 103
};

static MarsMaterialBlending MarsMaterialBlendingFromInt(int val) {
    switch (val) {
        case 100:
            return OPAQUE;
        case 101:
            return MASKED;
        case 102:
            return TRANSLUCENT;
        case 103:
            return ADDITIVE;
        default:
            MLOGE("MarsMaterialBlendingFromInt warning: %d", val);
            return OPAQUE;
    }
}

enum MarsMaterialSide {
    BACK = 1029,
    FRONT = 1028,
    BOTH = 1032
};

static MarsMaterialSide MarsMaterialSideFromInt(int val) {
    switch (val) {
        case 1028:
            return FRONT;
        case 1029:
            return BACK;
        case 1032:
            return BOTH;
        default:
            MLOGE("MarsMaterialSideFromInt warning: %d", val);
            return BOTH;
    }
}

// 纹理坐标变换，好像没用;
struct MarsTextureTransform {
    Vec2 offset;
    float rotation;
    Vec2 scale;
};

// 对应unlit options
struct MarsMaterialOptions {
    
    virtual ~MarsMaterialOptions() {}
    std::string name;
    MarsMaterialType type;
    
    Vec4 base_color_factor;
    std::shared_ptr<MarsTexture> base_color_texture;
    std::shared_ptr<MarsTextureTransform> base_color_texture_transform;
    size_t base_color_texture_coordinate = 0;
    
    MarsMaterialBlending blending;
    MarsMaterialSide side;
    float alpha_cut_off;        // todo: alpha测试值？
};

struct MarsMaterialPBROptions : public MarsMaterialOptions {
    
    float metallic_factor;      // 金属度系数，范围在[0, 1]之内。
    float roughness_factor;     // 粗超度系数，范围在[0, 1]之内。
    std::shared_ptr<MarsTexture> metallic_roughness_texture;
    std::shared_ptr<MarsTextureTransform> metallic_roughness_transform;
    size_t metallic_roughness_texture_coordinate;
    
    std::shared_ptr<MarsTexture> normal_texture;
    float normal_texture_scale;     // 法线贴图强度系数，范围在[0, 1]之内。
    std::shared_ptr<MarsTextureTransform> normal_texture_transform;
    size_t normal_texture_coordinate;
    
    std::shared_ptr<MarsTexture> occlusion_texture;
    float occlusion_texture_strength;
    std::shared_ptr<MarsTextureTransform> occlusion_texture_transform;
    size_t occlusion_texture_coordinate;
    
    Vec4 emissive_factor;
    float emissive_intensity;
    std::shared_ptr<MarsTexture> emissive_texture;
    std::shared_ptr<MarsTextureTransform> emissive_texture_transform;
    size_t emissive_texture_coordinate;
    
    // todo: 目前不处理shadow逻辑
    // bool enable_shadow;
};

struct MarsPrimitiveOptions {
    std::shared_ptr<MarsGeometry> geometry;
    std::shared_ptr<MarsMaterialOptions> material_options;
};

struct MarsMeshOptions : public BaseContentOptions{
    int parent;
    std::shared_ptr<MarsSkinOptions> skin;
    std::vector<std::shared_ptr<MarsPrimitiveOptions>> primitives;
    bool hide;
};

struct MarsItemMesh : public MarsItemModelBase {
    std::shared_ptr<MarsMeshOptions> options;
};

struct MarsCameraOption : public BaseContentOptions {
    int parent;
    float aspect; // 如果没有设置，会根据画布宽高自动计算
    float near;
    float far;
    float fov;
    int clip_mode;
};

struct MarsItemCamera : public MarsItemModelBase {
    struct {
        std::shared_ptr<MarsCameraOption> options;
    } content;
};

struct MarsLightOption : public BaseContentOptions {
    int parent;
    Vec4 color;         // 灯光颜色，线性空间，默认是[1, 1, 1]。??? 为什么是vec4；
    float intensity;    // 灯光强度，默认是1。
    std::string type;
    float range;
    float inner_cone_angle;
    float outer_cone_angle;
    
    virtual ~MarsLightOption() {}
};

struct MarsItemLight : public MarsItemModelBase {
    std::shared_ptr<MarsLightOption> options;
};

struct MarsSkyboxOptions {
    float intensity;
    float reflection_intensity;
    
    std::shared_ptr<MarsTexture> diffuse_image;
    std::shared_ptr<MarsTexture> specular_image;
    
    std::vector<std::vector<float>> irradiance_coeffs;
    
    float specular_image_size;
    int specular_mip_count;
};

struct MarsItemSkybox : public MarsItemModelBase {
    std::shared_ptr<MarsSkyboxOptions> options;
};

// Animations;
struct MarsAnimTrackOptions {
    int node;
    uint8_t* input; // no copy
    size_t input_byte_length;
    
    uint8_t* output; // no copy
    size_t output_byte_length;
    
    std::string path;               //'translation' | 'rotation' | 'scale' | 'weights',
    std::string interpolation;      //'LINEAR' | 'STEP' | 'CUBICSPLINE'
};

struct MarsAnimationOptions {
    std::string name;
    std::vector<std::shared_ptr<MarsAnimTrackOptions>> tracks;
};

struct TreeOptionsAnimEx : public TreeOptions {
    int animation;
    std::vector<std::shared_ptr<MarsAnimationOptions>> animation_options;   // 动画数据
};

// old data to remove;
enum MAnimMethodType {
    LINEAR,
    STEP,
    CUBICSPLINE
};

enum MAnimTargetType {
    TRANSLATION,
    ROTATION,
    SCALE
};

struct GeometryArribute : public AttributeBase {
    int buffer;  // index to MarsGLTFLoader.buffers;
    int buffer_offset;
    int buffer_length;
    BufferDataType type;
};

struct MarsGeometryParams {
    std::string name;
    int draw_start;
    int draw_count;
    std::unordered_map<std::string, std::shared_ptr<GeometryArribute>> attributes;
    GLenum mode;
    std::shared_ptr<GeometryArribute> index;
    int skeleton;
    
    // todo:盒子;
    // bounding: MarsItemBounding;
};

}

#endif /* common_hpp */
