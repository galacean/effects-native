//
//  model_data.h
//
//  Created by changxing on 2022/10/13.
//

#ifndef model_data_h
#define model_data_h

#include "sceneData/data/contents.h"
#include "sceneData/data/item.h"
#include "model/common.hpp"
#include "player/composition.h"
#include "util/util.hpp"

namespace mn {

struct BinaryData {
    void* data_ref = nullptr;
    int length = 0;
};

struct GLTFCubeTextureMipmapData {
    std::vector<BinaryData*> datas;
    std::shared_ptr<MarsTexture> texture;
    
    ~GLTFCubeTextureMipmapData() {
        for (int i = 0; i < datas.size(); i++) {
            delete datas[i];
        }
    }
};

struct GLTFTextureData {
    MInt* mag_filter = nullptr;
    MInt* min_filter = nullptr;
    MInt* wrap_s = nullptr;
    MInt* wrap_t = nullptr;
    MInt* source = nullptr;
    MInt* image_file_id = nullptr;
    MInt* target = nullptr;
    MInt* source_type = nullptr;
    MInt* format = nullptr;
    MInt* interal_format = nullptr;
    MInt* type = nullptr;
//    bool flip_y = false;
    GLTFCubeTextureMipmapData* cube_mipmaps = nullptr;
    
    ~GLTFTextureData() {
        MN_SAFE_DELETE(mag_filter);
        MN_SAFE_DELETE(min_filter);
        MN_SAFE_DELETE(wrap_s);
        MN_SAFE_DELETE(wrap_t);
        MN_SAFE_DELETE(source);
        MN_SAFE_DELETE(image_file_id);
        MN_SAFE_DELETE(cube_mipmaps);
        MN_SAFE_DELETE(target);
        MN_SAFE_DELETE(source_type);
        MN_SAFE_DELETE(format);
        MN_SAFE_DELETE(interal_format);
        MN_SAFE_DELETE(type);
    }
    
    std::shared_ptr<MarsTexture> ToMarsTexture(Composition* composition);
};

// mesh begin
struct GLTFBufferAttributeData {
    void* array = nullptr;
    int array_length = 0;
    int item_size = 0;
    int type = 0;
    int stride = 0;
    int offset = 0;
    bool normalize = false;

    std::shared_ptr<GPUBufferOption> ToOption() {
        BufferDataType buffer_data_type = Utils::ConvertDecoderTypeToBufferDataType(type);
        auto ret = std::make_shared<GPUBufferOption>(buffer_data_type);
        ret->item_size = item_size;
        ret->SetData((uint8_t*) array, array_length);
        return ret;
    }
};

struct ModelGeometryData {
    std::unordered_map<std::string, GLTFBufferAttributeData*> attributes;
    GLTFBufferAttributeData* indices = nullptr;
    int mode = 0;
    int draw_start = 0;
    MInt* draw_count = nullptr;
    MInt* instance_count = nullptr;
    MUint* buffer_usage = nullptr;
    
    ~ModelGeometryData() {
        for (auto iter : attributes) {
            if (iter.second) {
                delete iter.second;
            }
        }
        MN_SAFE_DELETE(indices);
        MN_SAFE_DELETE(draw_count);
        MN_SAFE_DELETE(instance_count);
        MN_SAFE_DELETE(buffer_usage);
    }

    std::shared_ptr<MarsGeometry> ToGeometry(MarsRenderer* renderer) {
        std::shared_ptr<MarsGeometry> geo = std::make_shared<MarsGeometry>(renderer, draw_count ? draw_count->val : -1, draw_start, mode);
        if (indices) {
            geo->SetIndexInfo(indices->ToOption());
        }
        
        for (auto iter : attributes) {
            const auto& attr = iter.second;
            std::string attribute_name;

            if (iter.first == "POSITION" || iter.first == "a_Position") {
                attribute_name = "a_Position";
            } else if (iter.first == "NORMAL" || iter.first == "a_Normal") {
                attribute_name = "a_Normal";
            } else if (iter.first == "TANGENT" || iter.first == "a_Tangent") {
                attribute_name = "a_Tangent";
            } else if (iter.first == "TEXCOORD_0" || iter.first == "a_UV1") {
                attribute_name = "a_UV1";
            } else if (iter.first == "TEXCOORD_1" || iter.first == "a_UV2") {
                attribute_name = "a_UV2";
            } else if (iter.first == "JOINTS_0" || iter.first == "a_Joint1") {
                attribute_name = "a_Joint1";
            } else if (iter.first == "WEIGHTS_0" || iter.first == "a_Weight1") {
                attribute_name = "a_Weight1";
            }
            
            AttributeWithData data(attr->item_size, 0, 0, attr->type);
            data.SetData((uint8_t*) attr->array, attr->array_length);
            geo->SetAttributeInfo(attribute_name, data);
        }
        return geo;
    }
};

struct MaterialOptionsData {
    std::string name;
    MarsMaterialType type = MarsMaterialType::UNLit;
    float* base_color_factor = nullptr;
    GLTFTextureData* base_color_texture = nullptr;
    std::shared_ptr<MarsTextureTransform> base_color_texture_transform = nullptr;
    MFloat* base_color_texture_coordinate = nullptr;
    MUint* blending = nullptr;
    MUint* side = nullptr;
    MFloat* alpha_cut_off = nullptr;
        
    virtual ~MaterialOptionsData() {
        MN_SAFE_DELETE(base_color_texture);
//        MN_SAFE_DELETE(base_color_texture_transform);
        MN_SAFE_DELETE(base_color_texture_coordinate);
        MN_SAFE_DELETE(blending);
        MN_SAFE_DELETE(side);
        MN_SAFE_DELETE(alpha_cut_off);
    }

    virtual std::shared_ptr<MarsMaterialOptions> ToMaterial(std::shared_ptr<MarsMaterialOptions> dst, Composition* composition) {
        if (!dst) {
            dst = std::make_shared<MarsMaterialOptions>();
        }
        dst->name = name;
        dst->type = type;
        if (base_color_factor) {
            dst->base_color_factor.Set(base_color_factor[0] / 255.0f, base_color_factor[1] / 255.0f, base_color_factor[2] / 255.0f, base_color_factor[3] / 255.0f);
        }
        if (base_color_texture) {
            dst->base_color_texture = base_color_texture->ToMarsTexture(composition);
        }
        if (base_color_texture_transform) {
            dst->base_color_texture_transform = base_color_texture_transform;
        }
        if (base_color_texture_coordinate) {
            dst->base_color_texture_coordinate = base_color_texture_coordinate->val;
        }
        if (blending) {
            dst->blending = MarsMaterialBlendingFromInt(blending->val);
        }
        if (side) {
            dst->side = MarsMaterialSideFromInt(side->val);
        }
        if (alpha_cut_off) {
            dst->alpha_cut_off = alpha_cut_off->val;
        }
        return dst;
    }
};

struct MaterialUnlitOptionsData : public MaterialOptionsData {
    
};

struct MaterialPBROptionsData : public MaterialOptionsData {
    float metallic_factor;
    float roughness_factor;
    
    GLTFTextureData* metallic_roughness_texture = nullptr;
    std::shared_ptr<MarsTextureTransform> metallic_roughness_texture_transform = nullptr;
    MFloat* metallic_roughness_texture_coordinate = nullptr;

    GLTFTextureData* normal_texture = nullptr;
    MFloat* normal_texture_scale = nullptr;
    std::shared_ptr<MarsTextureTransform> normal_texture_transform = nullptr;
    MFloat* normal_texture_coordinate = nullptr;

    GLTFTextureData* occlusion_texture = nullptr;
    MFloat* occlusion_texture_strength = nullptr;
    std::shared_ptr<MarsTextureTransform> occlusion_texture_transform = nullptr;
    MFloat* occlusion_texture_coordinate = nullptr;

    Vec4 emissive_factor;
    float emissive_intensity = 0;
    GLTFTextureData* emissive_texture = nullptr;
    std::shared_ptr<MarsTextureTransform> emissive_texture_transform = nullptr;
    MFloat* emissive_texture_coordinate = nullptr;

    MBool* enable_shadow = nullptr;
    
    
    ~MaterialPBROptionsData() {
        MN_SAFE_DELETE(metallic_roughness_texture);
//        MN_SAFE_DELETE(metallic_roughness_texture_transform);
        MN_SAFE_DELETE(metallic_roughness_texture_coordinate);
        MN_SAFE_DELETE(normal_texture);
        MN_SAFE_DELETE(normal_texture_scale);
//        MN_SAFE_DELETE(normal_texture_transform);
        MN_SAFE_DELETE(normal_texture_coordinate);
        MN_SAFE_DELETE(occlusion_texture);
        MN_SAFE_DELETE(occlusion_texture_strength);
//        MN_SAFE_DELETE(occlusion_texture_transform);
        MN_SAFE_DELETE(occlusion_texture_coordinate);
        MN_SAFE_DELETE(emissive_texture);
//        MN_SAFE_DELETE(emissive_texture_transform);
        MN_SAFE_DELETE(emissive_texture_coordinate);
        MN_SAFE_DELETE(enable_shadow);
    }
    
    std::shared_ptr<MarsMaterialOptions> ToMaterial(std::shared_ptr<MarsMaterialOptions> dst, Composition* composition) override {
        dst = std::make_shared<MarsMaterialPBROptions>();
        MaterialOptionsData::ToMaterial(dst, composition);
        dst->type = MarsMaterialType::PBR;
        std::shared_ptr<MarsMaterialPBROptions> pbr = std::static_pointer_cast<MarsMaterialPBROptions>(dst);
        
        pbr->metallic_factor = metallic_factor;
        pbr->roughness_factor = roughness_factor;
        if (metallic_roughness_texture) {
            pbr->metallic_roughness_texture = metallic_roughness_texture->ToMarsTexture(composition);
        }
        if (metallic_roughness_texture_transform) {
            pbr->metallic_roughness_transform = metallic_roughness_texture_transform;
        }
        if (metallic_roughness_texture_coordinate) {
            pbr->metallic_roughness_texture_coordinate = metallic_roughness_texture_coordinate->val;
        }
        if (normal_texture) {
            pbr->normal_texture = normal_texture->ToMarsTexture(composition);
        }
        if (normal_texture_scale) {
            pbr->normal_texture_scale = normal_texture_scale->val;
        }
        if (normal_texture_transform) {
            pbr->normal_texture_transform = normal_texture_transform;
        }
        if (normal_texture_coordinate) {
            pbr->normal_texture_coordinate = normal_texture_coordinate->val;
        }
        if (occlusion_texture) {
            pbr->occlusion_texture = occlusion_texture->ToMarsTexture(composition);
        }
        if (occlusion_texture_strength) {
            pbr->occlusion_texture_strength = occlusion_texture_strength->val;
        }
        if (occlusion_texture_transform) {
            pbr->occlusion_texture_transform = occlusion_texture_transform;
        }
        if (occlusion_texture_coordinate) {
            pbr->occlusion_texture_coordinate = occlusion_texture_coordinate->val;
        }
        pbr->emissive_factor.Set(emissive_factor.m[0] / 255.0f, emissive_factor.m[1] / 255.0f, emissive_factor.m[2] / 255.0f, emissive_factor.m[3] / 255.0f);
        pbr->emissive_intensity = emissive_intensity;
        if (emissive_texture) {
            pbr->emissive_texture = emissive_texture->ToMarsTexture(composition);
        }
        if (emissive_texture_transform) {
            pbr->emissive_texture_transform = emissive_texture_transform;
        }
        if (emissive_texture_coordinate) {
            pbr->emissive_texture_coordinate = emissive_texture_coordinate->val;
        }
        return dst;
    }
};

struct PrimitiveOptionsData {
    ModelGeometryData* geometry = nullptr;
    MaterialOptionsData* material = nullptr;
    
    ~PrimitiveOptionsData() {
        MN_SAFE_DELETE(geometry);
        MN_SAFE_DELETE(material);
    }
    
    void CopyTo(std::shared_ptr<MarsPrimitiveOptions> dst, Composition* composition) {
        if (geometry) {
            dst->geometry = geometry->ToGeometry(composition->renderer_);
        }
        if (material) {
            dst->material_options = material->ToMaterial(nullptr, composition);
        }
    }
};

struct SkinOptionsData {
    std::string name;
    int* joints = nullptr; // no copy
    int joints_length = 0;
    MInt* skeleton = nullptr;
    float* inverse_bind_matrices = nullptr; // no copy
    int inverse_bind_matrices_length = 0;
    
    ~SkinOptionsData() {
        MN_SAFE_DELETE(skeleton);
    }
    
    void CopyTo(std::shared_ptr<MarsSkinOptions> dst) {
        dst->name = name;
        for (int i = 0; i < joints_length; i++) {
            dst->joints.push_back(joints[i]);
        }
        dst->skeleton = skeleton != nullptr ? skeleton->val : -1;
        dst->matrices_byte_length = inverse_bind_matrices_length;
        dst->inverse_bind_matrices = (uint8_t*)inverse_bind_matrices;
    }
};

struct ModelMeshOptionsData {
    SkinOptionsData* skin = nullptr;
    std::vector<PrimitiveOptionsData*> primitives;
    MInt* parent = nullptr;
    
    ~ModelMeshOptionsData() {
        MN_SAFE_DELETE(skin);
        for (int i = 0; i < primitives.size(); i++) {
            MN_SAFE_DELETE(primitives[i]);
        }
        MN_SAFE_DELETE(parent);
    }
};

struct ModelMeshContentData : public ContentDataBase {
    ModelMeshOptionsData* options = nullptr;
    
    ~ModelMeshContentData() {
        MN_SAFE_DELETE(options);
    }
};
// mesh end

// tree begin
struct ModelAnimTrackOptionsData {
    int node = 0;
    uint8_t* input = nullptr; // no copy
    int input_length = 0;
    uint8_t* output = nullptr; // no copy
    int output_length = 0;
    std::string path;
    std::string interpolation;
    
    ~ModelAnimTrackOptionsData() {
    }
};

struct ModelAnimationOptionsData {
    std::string name;
    std::vector<ModelAnimTrackOptionsData*> tracks;
    
    ~ModelAnimationOptionsData() {
        for (int i = 0; i < tracks.size(); i++) {
            MN_SAFE_DELETE(tracks[i]);
        }
    }
};

struct TreeNodeOptionsData {
    std::string name;
    ItemTransformData* transform = nullptr;
    int* children = nullptr; // no copy
    int children_length = 0;
    std::string id;
    
    ~TreeNodeOptionsData() {
        MN_SAFE_DELETE(transform);
    }
};

struct ModelTreeOptionsData {
    int animation = 0;
    std::vector<ModelAnimationOptionsData*> animations;
    std::vector<TreeNodeOptionsData*> nodes;
    int* children = nullptr; // no copy
    int children_length = 0;
    
    ~ModelTreeOptionsData() {
        for (int i = 0; i < animations.size(); i++) {
            MN_SAFE_DELETE(animations[i]);
        }
        for (int i = 0; i < nodes.size(); i++) {
            MN_SAFE_DELETE(nodes[i]);
        }
    }
};

struct ModelTreeItemOptionsData {
    ModelTreeOptionsData* tree = nullptr;
    
    ~ModelTreeItemOptionsData() {
        MN_SAFE_DELETE(tree);
    }
};

struct ModelTreeContentData : public ContentDataBase {
    ModelTreeItemOptionsData* options = nullptr;
    
    ~ModelTreeContentData() {
        MN_SAFE_DELETE(options);
    }
};
// tree end

// skybox begin
struct SkyboxOptionsData {
    bool renderable = false;
    float intensity = 0;
    float reflections_intensity = 0;
    std::vector<std::vector<float>> irradianceCoeffs;
    GLTFTextureData* diffuse_image = nullptr;
    GLTFTextureData* specular_image = nullptr;
    float specular_image_size = 0;
    int specular_mip_count = 0;
    
    ~SkyboxOptionsData() {
        MN_SAFE_DELETE(diffuse_image);
        MN_SAFE_DELETE(specular_image);
    }
};

struct SkyboxContentData : public ContentDataBase {
    SkyboxOptionsData* options = nullptr;
    
    ~SkyboxContentData() {
        MN_SAFE_DELETE(options);
    }
};
// skybox end

// light begin
struct ModelLightOptionsData {
    std::string light_type;
    MInt* parent = nullptr;
    std::vector<float> color;
    float intensity = 0;
    float range = 0;
    float innerConeAngle = 0;
    float outerConeAngle = 0;
    
    ~ModelLightOptionsData() {
        MN_SAFE_DELETE(parent);
//        MN_SAFE_DELETE_ARR(color);
    }
};

struct ModelLightContentData : public ContentDataBase {
    ModelLightOptionsData* options = nullptr;
    
    ~ModelLightContentData() {
        MN_SAFE_DELETE(options);
    }
};
// light end

}

#endif /* model_data_h */
