//
//  particle_mesh.cpp
//
//  Created by Zongming Liu on 2022/4/12.
//

#include "particle_mesh.hpp"
#include <string>
#include <algorithm>
#include <unordered_map>
#include "util/log_util.hpp"
#include "util/color_utils.hpp"

#include "math/math_util.hpp"
#include "math/translate.h"
#include "math/value/curve_value_getter.hpp"

#include "shader/particle_shader.hpp"
#include "shader/integrate.hpp"
#include "shader/value.hpp"
#include "shader/blend.hpp"
#include "ri/render/mars_material.hpp"
#include "ri/render/mars_renderer.hpp"
#include "ri/material.h"
#include "particle_system.hpp"

namespace mn {

const size_t POS_ATTRIBUTEDATA_LENGTH = 4 * 12;
const size_t ROT_ATTRIBUTEDATA_LENGTH = 4 * 8;
const size_t OFFSET_ATTRIBUTEDATA_LENGTH = 4 * 4;
const size_t SPRITE_ATTRIBUTEDATA_LENGTH = 3 * 4;

const char* POS_ATTR_STRING = "aPos";
const char* ROT_ATTR_STRING = "aRot";
const char* OFFSET_ATTR_STRING = "aOffset";
const char* SPRITE_ATTR_STRING = "aSprite";
const char* VEL_ATTR_STRING = "aVel";
const char* DIR_X_ATTR_STRING = "aDirX";
const char* DIR_Y_ATTR_STRING = "aDirY";
const char* SEED_ATTR_STRING = "aSeed";
const char* COLOR_ATTR_STRING = "aColor";

struct PointData {
    float* a_pos = new float[POS_ATTRIBUTEDATA_LENGTH];
    float* a_rot = new float[ROT_ATTRIBUTEDATA_LENGTH];
    float* a_offset = new float[OFFSET_ATTRIBUTEDATA_LENGTH];
    float* a_sprite = nullptr;
    
    ~PointData() {
        MN_SAFE_DELETE_ARR(a_pos);
        MN_SAFE_DELETE_ARR(a_rot);
        MN_SAFE_DELETE_ARR(a_offset);
        MN_SAFE_DELETE_ARR(a_sprite);
    }
};

class ParticleTranslateTarget : public TranslateTarget {
public:
    ParticleTranslateTarget(ParticleMeshConstructor* particle) : particle_(particle) {
    }
    
    ValueGetter* GetSpeedOverLifetime() override {
        return particle_->speed_over_lifetime;
    }

    ValueGetter* GetGravityModifier() override {
        return particle_->gravity_modifier_ref;
    }

    LinearVelOverLifetime* GetLinearVelOverLifetime() override {
        return particle_->linear_vel_over_lifetime;
    }
    
    OrbitalVelOverLifetime* GetOrbitalVelOverLifetime() override {
        return particle_->orbital_vel_over_lifetime;
    }

private:
    ParticleMeshConstructor* particle_;
};

static void EnlargeBuffer(std::shared_ptr<GPUBufferOption> buffer_data, size_t length, float increase, size_t max_size) {
    size_t size = ceil(length * increase);
    if (buffer_data) {
        if (max_size > 0) {
            size = std::min(size, max_size);
        }
        
        buffer_data->EnlargeBuffer(size);
    } else {
        MLOGE("EnlargeBufferData Failed");
    }
}

ParticleMesh::ParticleMesh(ParticleMeshConstructor* props, MeshRendererOptions* options) : props_ref_(props) {
    MaterialOptions material_option(std::make_shared<MaterialRenderStates>());
    
    const auto renderer = options->renderer;
    material_option.shader.macros.push_back("#define RENDER_MODE " + std::to_string(props->render_mode ? props->render_mode->val : 0));
    if (props->diffuse->GetInnerTexture()->GetPremultiplyAlpha()) {
        material_option.shader.macros.push_back("#define PRE_MULTIPLY_ALPHA true");
    }
    
    int vertex_lookup_texture = 0;
    int frag_lookup_texture = 0;
    int shader_cache_id = 0;
    KeyFrameMeta vertex_key_frame_meta;
    KeyFrameMeta frag_key_frame_meta;
    bool enable_vertex_texture = renderer->InnerRenderer()->GPUCapability()->GetMaxVertexUniforms() > 0;
    
    if (enable_vertex_texture) {
        material_option.shader.macros.push_back("#define ENABLE_VERTEX_TEXTURE");
    }
    std::shared_ptr<UniformValue> u_speed_lifetime_value_data;
    if (props->speed_over_lifetime) {
        material_option.shader.macros.push_back("#define SPEED_OVER_LIFETIME");
        shader_cache_id |= 1 << 1;
        uint8_t* data = (uint8_t*) malloc(UNIFORM_VEC4_BYTESIZE);
        memset(data, 0, UNIFORM_VEC4_BYTESIZE);
        props->speed_over_lifetime->ToUniform(vertex_key_frame_meta,(float *) data);
        u_speed_lifetime_value_data = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
        u_speed_lifetime_value_data->ResetData(data);
    }
    const auto sprite = props->sprite_ref;
    std::shared_ptr<UniformValue> u_sprite_data;
    std::shared_ptr<UniformValue> u_f_sprite_data;
    if (sprite && sprite->animate) {
        material_option.shader.macros.push_back("#define USE_SPRITE");
        shader_cache_id |= 1 << 2;
        uint8_t* data = (uint8_t*) malloc(UNIFORM_VEC4_BYTESIZE);
        memset(data, 0, UNIFORM_VEC4_BYTESIZE);
        float* data_view = (float*) data;
        data_view[0] = sprite->col;
        data_view[1] = sprite->row;
        data_view[2] = sprite->total;
        data_view[3] = sprite->blend ? 1 : 0;
        u_sprite_data = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
        u_sprite_data->ResetData(data);

        uint8_t* frag_data = (uint8_t*) malloc(UNIFORM_VEC4_BYTESIZE);
        memset(frag_data, 0, UNIFORM_VEC4_BYTESIZE);
        float* frag_data_view = (float*) frag_data;
        frag_data_view[0] = sprite->col;
        frag_data_view[1] = sprite->row;
        frag_data_view[2] = sprite->total;
        frag_data_view[3] = sprite->blend ? 1 : 0;
        u_f_sprite_data = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
        u_f_sprite_data->ResetData(frag_data);
        
        props->use_sprite = true;
    }
    
    // filter
    
    const auto color_over_lifetime = props->color_over_lifetime;
    std::shared_ptr<UniformValue> u_color_lifetime_data;
    if (color_over_lifetime && color_over_lifetime->color_steps.size() > 0) {
        material_option.shader.macros.push_back("#define COLOR_OVER_LIFETIME");
        shader_cache_id |= 1 << 4;

        ImageTextureData image_texture_data(128, 1);
        ColorUtils::ImageDataFromGradient(image_texture_data, color_over_lifetime->color_steps);
        TextureOption texture_option(image_texture_data.width, image_texture_data.height, TextureSourceType::DATA);
        texture_option.SetData(image_texture_data.data);

        std::shared_ptr<MarsTexture> color_texture = std::make_shared<MarsTexture>(renderer, texture_option);
        u_color_lifetime_data = std::make_shared<UniformValue>(UniformDataType::SAMPLER_2D);
        u_color_lifetime_data->texture = color_texture;
    }
    
    auto opacity_data = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
    {
        uint8_t* data = (uint8_t*) malloc(UNIFORM_VEC4_BYTESIZE);
        memset(data, 0, UNIFORM_VEC4_BYTESIZE);

        if (color_over_lifetime && color_over_lifetime->opacity) {
            color_over_lifetime->opacity->ToUniform(vertex_key_frame_meta, (float*)data);
        } else {
            float *data_view = (float *)data;
            data_view[1] = 1.0;
        }
        opacity_data->ResetData(data);
    }
    
    const auto linear_over_lifetime = props->linear_vel_over_lifetime;
    const auto orbital_over_lifetime = props->orbital_vel_over_lifetime;
    bool use_orbital_vel = false;
    std::shared_ptr<UniformValue> linear_x_data;
    std::shared_ptr<UniformValue> linear_y_data;
    std::shared_ptr<UniformValue> linear_z_data;
    std::shared_ptr<UniformValue> orbital_x_data;
    std::shared_ptr<UniformValue> orbital_y_data;
    std::shared_ptr<UniformValue> orbital_z_data;
    if (linear_over_lifetime->x) {
        uint8_t *data = (uint8_t *)malloc(UNIFORM_VEC4_BYTESIZE);
        memset(data, 0, UNIFORM_VEC4_BYTESIZE);
        linear_over_lifetime->x->ToUniform(vertex_key_frame_meta, (float *) data);
        linear_x_data = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
        linear_x_data->ResetData(data);
        material_option.shader.macros.push_back("#define LINEAR_VEL_X 1");
        shader_cache_id |= 1 << 7;
        linear_over_lifetime->enabled = true;
    } else {
        material_option.shader.macros.push_back("#define LINEAR_VEL_X 0");
    }
    if (linear_over_lifetime->y) {
        uint8_t *data = (uint8_t *)malloc(UNIFORM_VEC4_BYTESIZE);
        memset(data, 0, UNIFORM_VEC4_BYTESIZE);
        linear_over_lifetime->y->ToUniform(vertex_key_frame_meta, (float *)data);
        linear_y_data = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
        linear_y_data->ResetData(data);
        material_option.shader.macros.push_back("#define LINEAR_VEL_Y 1");
        shader_cache_id |= 1 << 8;
        linear_over_lifetime->enabled = true;
    } else {
        material_option.shader.macros.push_back("#define LINEAR_VEL_Y 0");
    }
    if (linear_over_lifetime->z) {
        uint8_t *data = (uint8_t *)malloc(UNIFORM_VEC4_BYTESIZE);
        memset(data, 0, UNIFORM_VEC4_BYTESIZE);
        linear_over_lifetime->z->ToUniform(vertex_key_frame_meta, (float *)data);
        linear_z_data = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
        linear_z_data->ResetData(data);
        material_option.shader.macros.push_back("#define LINEAR_VEL_Z 1");
        shader_cache_id |= 1 << 9;
        linear_over_lifetime->enabled = true;
    } else {
        material_option.shader.macros.push_back("#define LINEAR_VEL_Z 0");
    }
    if (orbital_over_lifetime->x) {
        uint8_t *data = (uint8_t *)malloc(UNIFORM_VEC4_BYTESIZE);
        memset(data, 0, UNIFORM_VEC4_BYTESIZE);
        orbital_over_lifetime->x->ToUniform(vertex_key_frame_meta, (float *)data);
        orbital_x_data = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
        orbital_x_data->ResetData(data);
        material_option.shader.macros.push_back("#define ORB_VEL_X 1");
        shader_cache_id |= 1 << 10;
        use_orbital_vel = true;
        orbital_over_lifetime->enabled = true;
    } else {
        material_option.shader.macros.push_back("#define ORB_VEL_X 0");
    }
    if (orbital_over_lifetime->y) {
        uint8_t *data = (uint8_t *)malloc(UNIFORM_VEC4_BYTESIZE);
        memset(data, 0, UNIFORM_VEC4_BYTESIZE);
        orbital_over_lifetime->y->ToUniform(vertex_key_frame_meta, (float *)data);
        orbital_y_data = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
        orbital_y_data->ResetData(data);
        material_option.shader.macros.push_back("#define ORB_VEL_Y 1");
        shader_cache_id |= 1 << 11;
        use_orbital_vel = true;
        orbital_over_lifetime->enabled = true;
    } else {
        material_option.shader.macros.push_back("#define ORB_VEL_Y 0");
    }
    if (orbital_over_lifetime->z) {
        uint8_t *data = (uint8_t *)malloc(UNIFORM_VEC4_BYTESIZE);
        memset(data, 0, UNIFORM_VEC4_BYTESIZE);
        orbital_over_lifetime->z->ToUniform(vertex_key_frame_meta, (float *)data);
        orbital_z_data = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
        orbital_z_data->ResetData(data);
        shader_cache_id |= 1 << 12;
        material_option.shader.macros.push_back("#define ORB_VEL_Z 1");
        use_orbital_vel = true;
        orbital_over_lifetime->enabled = true;
    } else {
        material_option.shader.macros.push_back("#define ORB_VEL_Z 0");
    }
    if (linear_over_lifetime->as_movement) {
        material_option.shader.macros.push_back("#define AS_LINEAR_MOVEMENT");
        shader_cache_id |= 1 << 5;
    }
    std::shared_ptr<UniformValue> u_orb_center_data;
    if (use_orbital_vel) {
        if (orbital_over_lifetime->as_rotation) {
            material_option.shader.macros.push_back("#define AS_ORBITAL_MOVEMENT");
            shader_cache_id |= 6;
        }

        uint8_t *orb_center_data = (uint8_t *)malloc(UNIFORM_VEC3_BYTESIZE);
        memset(orb_center_data, 0, UNIFORM_VEC3_BYTESIZE);

        if (orbital_over_lifetime->center) {
            orbital_over_lifetime->center->Copy((float *)orb_center_data);
        }
        u_orb_center_data = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC3);
        u_orb_center_data->ResetData(orb_center_data);
    }
    
    const auto size_over_lifetime = props->size_over_lifetime;
    std::shared_ptr<UniformValue> size_x_data;
    std::shared_ptr<UniformValue> size_y_data;
    if (size_over_lifetime) {
        if (size_over_lifetime->size_x) {
            material_option.shader.macros.push_back("#define SIZE_BY_LIFE 1");
            shader_cache_id |= 1 << 13;

            uint8_t *data = (uint8_t *)malloc(UNIFORM_VEC4_BYTESIZE);
            memset(data, 0, UNIFORM_VEC4_BYTESIZE);

            size_over_lifetime->size_x->ToUniform(vertex_key_frame_meta, (float *) data);
            size_x_data = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
            size_x_data->ResetData(data);

            if (size_over_lifetime->separate_axes) {
                material_option.shader.macros.push_back("#define SIZE_Y_BY_LIFE 1");
                shader_cache_id |= 1 << 14;

                uint8_t *data_y = (uint8_t *)malloc(UNIFORM_VEC4_BYTESIZE);
                memset(data_y, 0, UNIFORM_VEC4_BYTESIZE);

                size_over_lifetime->size_y->ToUniform(vertex_key_frame_meta, (float *) data_y);
                size_y_data = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
                size_y_data->ResetData(data_y);
            }
        }
    }
    
    const auto rot_over_lifetime = props->rotation_over_lifetime;
    std::shared_ptr<UniformValue> rot_z_data;
    std::shared_ptr<UniformValue> rot_x_data;
    std::shared_ptr<UniformValue> rot_y_data;
    if (rot_over_lifetime) {
        if (rot_over_lifetime->rot_z) {
            material_option.shader.macros.push_back("#define ROT_Z_LIFETIME 1");
            shader_cache_id |= 1 << 15;

            uint8_t *data = (uint8_t *)malloc(UNIFORM_VEC4_BYTESIZE);
            memset(data, 0, UNIFORM_VEC4_BYTESIZE);
            rot_over_lifetime->rot_z->ToUniform(vertex_key_frame_meta, (float *) data);

            rot_z_data = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
            rot_z_data->ResetData(data);
        }

        if (rot_over_lifetime->rot_x) {
            material_option.shader.macros.push_back("#define ROT_X_LIFETIME 1");
            shader_cache_id |= 1 << 16;

            uint8_t *data = (uint8_t *)malloc(UNIFORM_VEC4_BYTESIZE);
            memset(data, 0, UNIFORM_VEC4_BYTESIZE);
            rot_over_lifetime->rot_x->ToUniform(vertex_key_frame_meta, (float *) data);

            rot_x_data = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
            rot_x_data->ResetData(data);
        }

        if (rot_over_lifetime->rot_y) {
            material_option.shader.macros.push_back("#define ROT_Y_LIFETIME 1");
            shader_cache_id |= 1 << 17;

            uint8_t *data = (uint8_t *)malloc(UNIFORM_VEC4_BYTESIZE);
            memset(data, 0, UNIFORM_VEC4_BYTESIZE);
            rot_over_lifetime->rot_y->ToUniform(vertex_key_frame_meta, (float *) data);

            rot_y_data = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
            rot_y_data->ResetData(data);
        }

        if (rot_over_lifetime->as_rotation) {
            material_option.shader.macros.push_back("#define ROT_LIFETIME_AS_MOVEMENT 1");
            shader_cache_id |= 1 << 18;
        }
    }
    
    auto u_acceleration = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
    const auto gravity = props->gravity;
    {
        uint8_t* acceleration_data = (uint8_t *)malloc(UNIFORM_VEC4_BYTESIZE);
        memset(acceleration_data, 0, UNIFORM_VEC4_BYTESIZE);
        if (gravity) {
            gravity->Copy((float*) acceleration_data);
        }
        u_acceleration->ResetData(acceleration_data);
    }
    std::shared_ptr<UniformValue> u_gravity_modifier;
    {
        uint8_t *data = (uint8_t *)malloc(UNIFORM_VEC4_BYTESIZE);
        memset(data, 0, UNIFORM_VEC4_BYTESIZE);
        props->gravity_modifier_ref->ToUniform(vertex_key_frame_meta, (float*) data);
        u_gravity_modifier = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
        u_gravity_modifier->ResetData(data);
    }
    
    const auto force_opt = props->force_target;
    std::shared_ptr<UniformValue> u_final_target_data;
    std::shared_ptr<UniformValue> u_force_curve_data;
    if (force_opt) {
        material_option.shader.macros.push_back("#define FINAL_TARGET");
        shader_cache_id |= 1 << 19;
        {
            uint8_t* data = (uint8_t*) malloc(UNIFORM_VEC3_BYTESIZE);
            force_opt->target.Copy((float*) data);
            u_final_target_data = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC3);
            u_final_target_data->ResetData(data);
        }
        {
            uint8_t* data = (uint8_t*) malloc(UNIFORM_VEC4_BYTESIZE);
            memset(data, 0, UNIFORM_VEC4_BYTESIZE);
            force_opt->curve->ToUniform(vertex_key_frame_meta, (float*) data);
            u_force_curve_data = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
            u_force_curve_data->ResetData(data);
        }
    }
    
    // todo: halfFloatTexture
    bool HALF_FLOAT = false;
    std::shared_ptr<UniformValue> u_f_curve_values;
    if (HALF_FLOAT && frag_key_frame_meta.max) {
        assert(0);
    } else {
        u_f_curve_values = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4, (size_t) vertex_key_frame_meta.index);
        u_f_curve_values->ResetData(CurveValueGetter::GetAllData(vertex_key_frame_meta));
    }
    
    // todo: vertexCurveTexture
    std::shared_ptr<UniformValue> u_v_curve_values = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4, (size_t) vertex_key_frame_meta.index);
    u_v_curve_values->ResetData(CurveValueGetter::GetAllData(vertex_key_frame_meta));
    material_option.shader.macros.push_back("#define VERT_CURVE_VALUE_COUNT " + std::to_string(vertex_key_frame_meta.frame_datas.size()));
    material_option.shader.macros.push_back("#define FRAG_CURVE_VALUE_COUNT " + std::to_string(frag_key_frame_meta.frame_datas.size()));
    material_option.shader.macros.push_back("#define VERT_MAX_KEY_FRAME_COUNT " + std::to_string((int)vertex_key_frame_meta.max));
    material_option.shader.macros.push_back("#define FRAG_MAX_KEY_FRAME_COUNT " + std::to_string((int)frag_key_frame_meta.max));
    
    std::stringstream shader_cache;
    shader_cache << "p+" << std::to_string(props->render_mode ? props->render_mode->val : 0)
        << "+" << std::to_string(shader_cache_id)
        << "+" << std::to_string(vertex_key_frame_meta.index)
        << "+" << std::to_string(vertex_key_frame_meta.max)
        << "+" << std::to_string(frag_key_frame_meta.index)
        << "+" << std::to_string(frag_key_frame_meta.max);
    material_option.shader_cached_id = "p-:" + shader_cache.str();
    {
        std::stringstream vert_ss;
        vert_ss << "#define LOOKUP_TEXTURE_CURVE 0";
        if (props->orbital_vel_over_lifetime && props->orbital_vel_over_lifetime->use_highp_float_on_ios) {
            material_option.shader.fragment = SHADER_PRECISION_HIGHP + PARTICLE_FRAGMENT_SOURCE;
            vert_ss << SHADER_PRECISION_HIGHP;
        } else {
            material_option.shader.fragment = SHADER_PRECISION +PARTICLE_FRAGMENT_SOURCE;
            vert_ss << SHADER_PRECISION;
        }
        vert_ss << PARTICLE_VERTEX_SOURCE;
        material_option.shader.vertex = vert_ss.str();
    }
    material_option.shader.name = "particle#" + props->name;
    // filter
    
    std::shared_ptr<MaterialRenderStates> states = material_option.states;
    {
        int side = props->side ? props->side->val : SIDE_BOTH;
        switch (side) {
            case SIDE_BACK:
                states->cull_face = std::make_shared<MUint>(GL_BACK);
                states->front_face = std::make_shared<MUint>(GL_CW);
                states->cull_face_enabled = std::make_shared<MBool>(true);
                break;

            case SIDE_FRONT:
                states->cull_face = std::make_shared<MUint>(GL_FRONT);
                states->front_face = std::make_shared<MUint>(GL_CW);
                states->cull_face_enabled = std::make_shared<MBool>(true);
                break;

            default:
                states->cull_face_enabled = std::make_shared<MBool>(false);
                break;
        }
    }
    states->depth_test = std::make_shared<MBool>(true);
    states->depth_mask = std::make_shared<MBool>(props->occlusion);
    
    mesh_ = std::make_shared<MarsMesh>(renderer, props->name);
    mesh_->priority_ = props->list_index;
    
    int pre_multi_alpha = MaterialUtil::SetMtlBlending(
        CreateBlendModeType(props->blending ? props->blending->val : 0),
        *material_option.states.get());
    
    MaterialUtil::SetMtlStencil(
        CreateMaskModeType(props->mask_mode ? props->mask_mode->val : 0),
        props->mask ? props->mask->val : 0,
        *material_option.states.get());
    mesh_->SetMaterial(material_option);
    auto material = mesh_->GetMaterial();
    
    material->SetUniformValue("uViewProjection", std::make_shared<UniformValue>(FLOAT_MAT4, true, "uViewProjection"));
    material->SetUniformValue("uViewP", std::make_shared<UniformValue>(FLOAT_MAT4, true, "uViewP"));
    material->SetUniformValue("uModel", std::make_shared<UniformValue>(FLOAT_MAT4, true, "uModel"));
    
    std::shared_ptr<MarsTexture> diffuse = props->diffuse ?
        props->diffuse : MaterialUtil::CreateEmptyTextureContainer(renderer);
    props->diffuse = diffuse;
    diffuse->AssignRenderer(renderer);
    {
        auto u_tex_offset = std::make_shared<UniformValue>(FLOAT_VEC2);
        float* u_tex_offset_data = new float[2];
        if (diffuse) {
            u_tex_offset_data[0] = 1.0f / (float) diffuse->Width();
            u_tex_offset_data[1] = 1.0f / (float) diffuse->Height();
        } else {
            u_tex_offset_data[0] = 0;
            u_tex_offset_data[1] = 0;
        }
        u_tex_offset->ResetData(u_tex_offset_data);
        material->SetUniformValue("uTexOffset", u_tex_offset);
    }
    {
        auto u_params = std::make_shared<UniformValue>(FLOAT_VEC4);
        float* u_params_data = new float[4]{0, props->duration, 0, 0};
        u_params->ResetData(u_params_data);
        material->SetUniformValue("uParams", u_params);
    }
    auto u_mask_tex = std::make_shared<UniformValue>(SAMPLER_2D);
    u_mask_tex->SetTexture(diffuse);
    material->SetUniformValue("uMaskTex", u_mask_tex);
    {
        float* color_params_data = (float*)malloc(UNIFORM_VEC4_BYTESIZE);
        memset(color_params_data, 0, UNIFORM_VEC4_BYTESIZE);
        color_params_data[0] = diffuse ? 1 : 0;
        color_params_data[1] = pre_multi_alpha;
        color_params_data[2] = 0;
        color_params_data[3] = (props->occlusion && !(props->transparent_occlusion && props->transparent_occlusion->val)) ? 1 : 0;
        auto u_color_params = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
        u_color_params->ResetData(color_params_data);
        material->SetUniformValue("uColorParams", u_color_params);
    }
    
    if (u_speed_lifetime_value_data) {
        material->SetUniformValue("uSpeedLifetimeValue", u_speed_lifetime_value_data);
    }
    if (u_sprite_data) {
        material->SetUniformValue("uSprite", u_sprite_data);
    }
    if (u_f_sprite_data) {
        material->SetUniformValue("uFSprite", u_f_sprite_data);
    }
    if (u_color_lifetime_data) {
        material->SetUniformValue("uColorOverLifetime", u_color_lifetime_data);
    }
    material->SetUniformValue("uOpacityOverLifetimeValue", opacity_data);
    if (linear_x_data) {
        material->SetUniformValue("uLinearXByLifetimeValue", linear_x_data);
    }
    if (linear_y_data) {
        material->SetUniformValue("uLinearYByLifetimeValue", linear_y_data);
    }
    if (linear_z_data) {
        material->SetUniformValue("uLinearZByLifetimeValue", linear_z_data);
    }
    if (orbital_x_data) {
        material->SetUniformValue("uOrbXByLifetimeValue", orbital_x_data);
    }
    if (orbital_y_data) {
        material->SetUniformValue("uOrbYByLifetimeValue", orbital_y_data);
    }
    if (orbital_z_data) {
        material->SetUniformValue("uOrbZByLifetimeValue", orbital_z_data);
    }
    if (u_orb_center_data) {
        material->SetUniformValue("uOrbCenter", u_orb_center_data);
    }
    if (size_x_data) {
        material->SetUniformValue("uSizeByLifetimeValue", size_x_data);
    }
    if (size_y_data) {
        material->SetUniformValue("uSizeYByLifetimeValue", size_y_data);
    }
    if (rot_z_data) {
        material->SetUniformValue("uRZByLifeTimeValue", rot_z_data);
    }
    if (rot_x_data) {
        material->SetUniformValue("uRXByLifeTimeValue", rot_x_data);
    }
    if (rot_y_data) {
        material->SetUniformValue("uRYByLifeTimeValue", rot_y_data);
    }
    material->SetUniformValue("uAcceleration", u_acceleration);
    if (u_gravity_modifier) {
        material->SetUniformValue("uGravityModifierValue", u_gravity_modifier);
    }
    if (u_final_target_data) {
        material->SetUniformValue("uFinalTarget", u_final_target_data);
    }
    if (u_force_curve_data) {
        material->SetUniformValue("uForceCurve", u_force_curve_data);
    }
    if (u_f_curve_values) {
        material->SetUniformValue("uFCurveValues", u_f_curve_values);
    }
    if (u_v_curve_values) {
        material->SetUniformValue("uVCurveValues", u_v_curve_values);
    }
    
    use_sprite_ = props->use_sprite;
    CreateGrometry(renderer, use_sprite_);
    if (props->matrix) {
        mesh_->world_matrix_.Set(*props->matrix);
    }
    
    if (props->texture_flip) {
        texture_offsets_ = {0, 0, 1, 0, 0, 1, 1, 1};
    } else {
        texture_offsets_ = {0, 1, 0, 0, 1, 1, 1, 0};
    }
    if (props->anchor) {
        anchor_.Set(props->anchor->m[0] - 0.5f, 0.5f - props->anchor->m[1]);
    }
    max_count_ = props->max_count;
}

ParticleMesh::~ParticleMesh() {
//    MLOGD("Particle Mesh Destructed");
}

void ParticleMesh::CreateGrometry(MarsRenderer* renderer, bool use_sprite) {    
    std::shared_ptr<MarsGeometry> geometry = std::make_shared<MarsGeometry>(renderer);
    int bpe = sizeof(float);
    int j12 = bpe * 12;
    
    geometry->SetAttributeInfo("aPos", AttributeWithData(3, j12, 0));
    geometry->SetAttributeInfo("aVel", AttributeWithData(3, j12, 3 * bpe, "aPos"));
    geometry->SetAttributeInfo("aDirX", AttributeWithData(3, j12, 6 * bpe, "aPos"));
    geometry->SetAttributeInfo("aDirY", AttributeWithData(3, j12, 9 * bpe, "aPos"));
    
    geometry->SetAttributeInfo("aRot", AttributeWithData(3, 8 * bpe, 0));
    geometry->SetAttributeInfo("aSeed", AttributeWithData(1, 8 * bpe, 3 * bpe, "aRot"));
    geometry->SetAttributeInfo("aColor", AttributeWithData(4, 8 * bpe, 4 * bpe, "aRot"));
    
    geometry->SetAttributeInfo("aOffset", AttributeWithData(4, 4 * bpe, 0));
    
    if (use_sprite) {
        geometry->SetAttributeInfo("aSprite", AttributeWithData(3, 3 * bpe, 0));
    }
    
    std::shared_ptr<GPUBufferOption> index_buffer = std::make_shared<GPUBufferOption>(UNSIGNED_SHORT);
    geometry->SetIndexInfo(index_buffer);
    mesh_->SetGeometry(geometry);
    geometry_ = geometry;
}

void ParticleMesh::SetPoint(std::unique_ptr<ParticlePoint> point, size_t index) {
//    MLOGD("ParticleMesh::SetPoint At index:%d", index);
    if (index < max_count_) {
        size_t particle_count = index + 1;
        size_t vertex_count = particle_count * 4;
        bool increase_buffer = particle_count > max_particle_buffer_count_;
        float inc = 1.0f;
        if (particle_count_ > 300) {
            inc = ((float) particle_count_ + 100.0f) / (float) particle_count_;
        } else if (particle_count_ > 100) {
            inc = 1.4f;
        } else if (particle_count_ > 0) {
            inc = 2;
        }
        
        PointData point_data;
        if (use_sprite_) {
            point_data.a_sprite = new float[SPRITE_ATTRIBUTEDATA_LENGTH];
        }
        
        auto& offsets = texture_offsets_;
        float off[4] = { 0.0, 0.0, point->delay, point->lifetime };
        float whole_uv[4] = { 0.0, 1.0, 0.0, 1.0 };
        auto& pos = point->pos;
        auto& vel = point->vel;
        auto& color = point->color;
        auto& rot = point->rot;
        Vec3* sprite = nullptr;
        
        if (use_sprite_) {
            sprite = &point->sprite;
        }
        float size_offsets[8] = { -0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, -0.5 };
        float seed = MathUtil::Random(0.0, 1.0);
        for (size_t j = 0; j < 4; j++) {
            size_t offset = j * 2;
            size_t j3 = j * 3;
            size_t j4 = j * 4;
            size_t j8 = j * 8;
            size_t j12 = j * 12;
            memcpy(point_data.a_pos + j12, pos.Data(), sizeof(float) * pos.Length());
            memcpy(point_data.a_pos + j12 + 3, vel.Data(), sizeof(float) * vel.Length());
            
            memcpy(point_data.a_rot + j8, rot.Data(), sizeof(float) * rot.Length());
            memcpy(point_data.a_rot + (j8 + 3), &seed, 1 * sizeof(float));
            // todo：统一颜色数值范围
            float* temp_color = new float[color.Length()];
            for (int ic = 0; ic < color.Length(); ic++) {
                temp_color[ic] = color.Data()[ic];
            }
            memcpy(point_data.a_rot + (j8 + 4), temp_color, sizeof(float) * color.Length());
            delete[] temp_color;
            
            if (use_sprite_) {
                memcpy(point_data.a_sprite + j3, sprite->Data(), sizeof(float) * sprite->Length());
            }
            auto& uv = point->uv;
            float uvy = use_sprite_ ? (1 - offsets[offset + 1]) : offsets[offset + 1];
            off[ 0 ] = uv.m[ 0 ]  + offsets[offset] * uv.m[ 2 ];
            off[ 1 ] = uv.m[ 1 ]  + uvy * uv.m[ 3 ];
            memcpy(point_data.a_offset + j4, off, sizeof(float) * 4);
            
            // 四个顶点通过sizeoffset 以及scale的大小来描述粒子不同的顶点；postion + offset;
            size_t ji = (j + j);
            float sx = (size_offsets[ji] - anchor_.m[0]) * point->size.m[0];
            float sy = (size_offsets[ji + 1] - anchor_.m[1]) * point->size.m[1];
            for (size_t k = 0; k < 3; k++) {
                point_data.a_pos[j12 + 6 + k] = point->dir_x.m[k] * sx;
                point_data.a_pos[j12 + 9 + k] = point->dir_y.m[k] * sy;
            }
        }
        
        float base_index[INDEX_ELEMENT_LENGTH] = { 0, 1, 2, 2, 1, 3 };
        uint16_t a_index_data[INDEX_ELEMENT_LENGTH] = { 0 };
        
        for (size_t i=0; i<INDEX_ELEMENT_LENGTH; i++) {
            a_index_data[i] = base_index[i] + 4 * index;
        }
        
        if (increase_buffer) {
            const auto& index_buffer_option = geometry_->GetIndexBufferOption();
            EnlargeBuffer(index_buffer_option, particle_count * INDEX_ELEMENT_LENGTH * BYTE_PER_U16_ELEMENT , inc,
                                max_count_ * INDEX_ELEMENT_LENGTH * BYTE_PER_U16_ELEMENT);
            index_buffer_option->SetOffsetData((uint8_t*)a_index_data, index * INDEX_ELEMENT_LENGTH * BYTE_PER_U16_ELEMENT, INDEX_ELEMENT_LENGTH * BYTE_PER_U16_ELEMENT);
            geometry_->NeedUpdateIndexData();
            this->max_particle_buffer_count_ = index_buffer_option->byte_length / (INDEX_ELEMENT_LENGTH * BYTE_PER_U16_ELEMENT);
        } else {
            mesh_->GetGeometry()->SetIndexSubData((uint8_t*)a_index_data, index * INDEX_ELEMENT_LENGTH * BYTE_PER_U16_ELEMENT, INDEX_ELEMENT_LENGTH * BYTE_PER_U16_ELEMENT);
        }
        
        if (increase_buffer) {
            const auto& pos_buffer_option = geometry_->GetAttributeBufferOption(POS_ATTR_STRING);
            size_t pos_attribute_stride = geometry_->GetAttributeStride(POS_ATTR_STRING);
            EnlargeBuffer(pos_buffer_option, vertex_count * pos_attribute_stride, inc, max_count_ * 4 * pos_attribute_stride);
            pos_buffer_option->SetOffsetData((uint8_t*)point_data.a_pos, index * POS_ATTRIBUTEDATA_LENGTH * BYTE_PER_ELEMENT, POS_ATTRIBUTEDATA_LENGTH * BYTE_PER_ELEMENT);
            geometry_->NeedUpdateAttributeData(POS_ATTR_STRING);
        } else {
            geometry_->SetAttributeSubData(POS_ATTR_STRING, (uint8_t*)point_data.a_pos, index * POS_ATTRIBUTEDATA_LENGTH * BYTE_PER_ELEMENT, POS_ATTRIBUTEDATA_LENGTH * BYTE_PER_ELEMENT);
        }
        
        if (increase_buffer) {
            const auto& rot_buffer_option = geometry_->GetAttributeBufferOption(ROT_ATTR_STRING);
            size_t rot_attribute_stride = geometry_->GetAttributeStride(ROT_ATTR_STRING);
            EnlargeBuffer(rot_buffer_option, vertex_count * rot_attribute_stride, inc, max_count_ * 4 * rot_attribute_stride);
            rot_buffer_option->SetOffsetData((uint8_t*)point_data.a_rot, index * ROT_ATTRIBUTEDATA_LENGTH * BYTE_PER_ELEMENT, ROT_ATTRIBUTEDATA_LENGTH * BYTE_PER_ELEMENT);
            geometry_->NeedUpdateAttributeData(ROT_ATTR_STRING);
        } else {
            geometry_->SetAttributeSubData(ROT_ATTR_STRING, (uint8_t*)point_data.a_rot, index * ROT_ATTRIBUTEDATA_LENGTH * BYTE_PER_ELEMENT, ROT_ATTRIBUTEDATA_LENGTH * BYTE_PER_ELEMENT);
        }
        
        if (increase_buffer) {
            const auto& offset_buffer_option = geometry_->GetAttributeBufferOption(OFFSET_ATTR_STRING);
            size_t offset_attribute_stride = geometry_->GetAttributeStride(OFFSET_ATTR_STRING);
            EnlargeBuffer(offset_buffer_option, vertex_count * offset_attribute_stride, inc, max_count_ * 4 * offset_attribute_stride);
            offset_buffer_option->SetOffsetData((uint8_t*)point_data.a_offset, index * OFFSET_ATTRIBUTEDATA_LENGTH * BYTE_PER_ELEMENT, OFFSET_ATTRIBUTEDATA_LENGTH * BYTE_PER_ELEMENT);
            geometry_->NeedUpdateAttributeData(OFFSET_ATTR_STRING);
        } else {
            geometry_->SetAttributeSubData(OFFSET_ATTR_STRING, (uint8_t*)point_data.a_offset, index * OFFSET_ATTRIBUTEDATA_LENGTH * BYTE_PER_ELEMENT, OFFSET_ATTRIBUTEDATA_LENGTH * BYTE_PER_ELEMENT);
        }
        
        
        if (use_sprite_) {
            if (increase_buffer) {
                auto sprite_buffer_option = geometry_->GetAttributeBufferOption(SPRITE_ATTR_STRING);
                size_t sprite_stride = geometry_->GetAttributeStride(SPRITE_ATTR_STRING);
                EnlargeBuffer(sprite_buffer_option, vertex_count * sprite_stride, inc, max_count_ * 4 * sprite_stride);
                sprite_buffer_option->SetOffsetData((uint8_t*)point_data.a_sprite, index * SPRITE_ATTRIBUTEDATA_LENGTH * BYTE_PER_ELEMENT,
                                                    SPRITE_ATTRIBUTEDATA_LENGTH * BYTE_PER_ELEMENT);
                geometry_->NeedUpdateAttributeData(SPRITE_ATTR_STRING);
            } else {
                geometry_->SetAttributeSubData(SPRITE_ATTR_STRING, (uint8_t*)point_data.a_sprite, index * SPRITE_ATTRIBUTEDATA_LENGTH * BYTE_PER_ELEMENT,
                                                   SPRITE_ATTRIBUTEDATA_LENGTH * BYTE_PER_ELEMENT);
            }
        }
        
        this->particle_count_ = std::max(this->particle_count_, particle_count);
        geometry_->SetDrawCount(this->particle_count_ * 6);
    }
}

float ParticleMesh::GetTime() {
    auto param_uniform_data = mesh_->GetMaterial()->GetUniformValue("uParams");
    if (param_uniform_data) {
        return ((float*) param_uniform_data->GetData())[0];
    } else {
        MLOGE("GetTime Faild for uniform Data is null");
        return 0;
    }
}

void ParticleMesh::SetTime(float now) {
    auto param_uniform_data = mesh_->GetMaterial()->GetUniformValue("uParams");
    if (param_uniform_data) {
        param_uniform_data->SetData(now, 0);
    } else {
        MLOGE("SetTime Faild for uniform Data is null");
    }
}

void ParticleMesh::ReverseTime(float time) {
    auto mesh_geometry = mesh_->GetGeometry();
    auto offset_buffer_option = mesh_geometry->GetAttributeBufferOption("aOffset");
    if (offset_buffer_option) {
        float* temp = (float*) offset_buffer_option->data;
        for (size_t i = 0; i < offset_buffer_option->byte_length / 4 / sizeof(float); i++) {
            offset_buffer_option->SetData(temp[4 * i + 2] - time, 4 * i + 2);
        }
    }
    mesh_geometry->NeedUpdateAttributeData("aOffset");
    SetTime(GetTime() - time);
}

void ParticleMesh::ClearPoints() {
    geometry_->ResetGeometryData();
    particle_count_ = 0;
    geometry_->SetDrawCount(0);
    max_particle_buffer_count_ = 0;
}

void ParticleMesh::RemovePoint(size_t index) {
    if (index < particle_count_) {
        auto offset_buffer_option = mesh_->GetGeometry()->GetAttributeBufferOption("aOffset");
        if (offset_buffer_option) {
            for (size_t i=0; i<16; i++) {
                offset_buffer_option->SetData(0.0, 16 * index + i);
            }
        }
    }
}

void ParticleMesh::GetPointPosition(Vec3& out, int index) {
    const auto geo = geometry_;
    const int pos_index = index * 48;
    const auto& pos_opt = geo->GetAttributeBufferOption("aPos");
    float* pos_data_arr = (float*) pos_opt->data;
    auto pos_data_len = pos_opt->byte_length / sizeof(float);
    float* offset_data = (float*) geo->GetAttributeBufferOption("aOffset")->data;
    const float time = GetTime() - offset_data[index * 16 + 2];
    const float point_dur = offset_data[index * 16 + 3];
    const auto mtl = mesh_->material_;
    {
        ParticleTranslateTarget target(props_ref_);
        float* acc_float_arr = (float*) mtl->GetUniformValue("uAcceleration")->GetData();
        Vec3 acc(acc_float_arr[0], acc_float_arr[1], acc_float_arr[2]);
        Vec3* pos_data = new Vec3[pos_data_len / 3];
        for (int i = 0; i < pos_data_len / 3; i++) {
            pos_data[i].Set(pos_data_arr[i * 3], pos_data_arr[i * 3 + 1], pos_data_arr[i * 3 + 2]);
        }
        Translate::CalculateTranslation(out, &target, acc, time, point_dur, pos_data, pos_data, pos_index, pos_index + 3);
        
        delete[] pos_data;
    }
    if (props_ref_->force_target) {
        float* target = (float*) mtl->GetUniformValue("uFinalTarget")->GetData();
        float life = props_ref_->force_target->curve->GetValue(time / point_dur);
        float dl = 1 - life;
        out.m[0] = out.m[0] * dl + target[0] * life;
        out.m[1] = out.m[1] * dl + target[1] * life;
        out.m[2] = out.m[2] * dl + target[2] * life;
    }
}

}
