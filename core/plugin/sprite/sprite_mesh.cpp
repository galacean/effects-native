#include "sprite_mesh.h"
#include "sprite_item.h"
#include "ri/render_base.hpp"
#include "ri/render/mars_mesh.hpp"
#include "ri/render/mars_texture.hpp"
#include "shader/sprite_shader.hpp"
#include "util/log_util.hpp"
#include "ri/material.h"
#include "math/translate.h"
#include "math/math_util.hpp"
#include "plugin/cal/calculate_group.h"
#include "extensions/morph/custom_morph.hpp"

namespace mn {

static int s_max_sprite_mesh_item_count_ = 8;

static int s_seed = 1;

void SpriteMesh::SetSpriteMeshMaxItemCountByGPU(std::shared_ptr<GLGPUCapability> gpu) {
    if (gpu && gpu->GetMaxFragmentTextures() >= 16 && gpu->GetMaxVertexUniforms() >= 128) {
        SetSpriteMeshMaxItemCount(16);
    } else {
        SetSpriteMeshMaxItemCount(8);
    }
}

void SpriteMesh::SetSpriteMeshMaxItemCount(int count) {
    s_max_sprite_mesh_item_count_ = count;
}

// todo: with options?: { env?: string, count?: number, ignoreBlend?: boolean, wireframe?: boolean }
void SpriteMesh::SpriteMeshShaderFromFilter(Shader& shader, FilterDefine* filter, int count) {
    std::string max_item_count = "#define MAX_ITEM_COUNT ";
    max_item_count.append(std::to_string(count));
    shader.macros.push_back(max_item_count);
    
    std::string adjust_layer = "#define ADJUST_LAYER ";
    if (filter) {
        adjust_layer.append("true");
        shader.macros.push_back(adjust_layer);
    }

    std::string use_blend = "#define USE_BLEND ";
    use_blend.append("true");
    shader.macros.push_back(use_blend);
    
    // todo: 10.3.0 版本 wireFrame?
    std::string vertex_shader = SPRITE_VERTEX_SOURCE;
    auto vert_find_result = vertex_shader.find("#pragma FILTER_VERT");
    if (filter && !filter->GetMeshFilterOptions().vertex.empty()) {
        if (vert_find_result != std::string::npos) {
            vertex_shader.replace(vert_find_result, 19, filter->GetMeshFilterOptions().vertex);
        } else {
            MLOGE("Filter Vertex Shader Replace Failed");
        }
    } else {
        if (vert_find_result != std::string::npos) {
            vertex_shader.replace(vert_find_result, 19, "vec4 filterMain(float t,vec4 pos){ return uViewProjection * pos; }");
        } else {
            MLOGE("Sprite Vertex Shader Replace Failed");
        }
    }
    
    std::string fragment_shader = SPRITE_FRAGMENT_SOURCE;
    auto frag_find_result = fragment_shader.find("#pragma FILTER_FRAG");
    if (filter) {
        if (frag_find_result != std::string::npos) {
            fragment_shader.replace(frag_find_result, 19, filter->GetMeshFilterOptions().fragment);
        } else {
            MLOGE("Filter Fragment Shader Replace Failed");
        }
    } else {
        if (frag_find_result != std::string::npos) {
            fragment_shader.replace(frag_find_result, 19, "");
        } else {
            MLOGE("Sprite Fragment Shader Replace Failed");
        }
    }
    
    shader.fragment = fragment_shader;
    shader.vertex = vertex_shader;
    shader.shaderd = true;
}

std::string SpriteMesh::SpriteMeshShaderFromRenderInfo(Shader& shader, SpriteItemRenderInfo* render_info, int count) {
    std::stringstream temp;
    if (!count) {
        count = 2;
    }
    
    if (render_info->premultiply_alpha) {
       shader.macros.push_back("#define PRE_MULTIPLY_ALPHA true");
   }
    SpriteMesh::SpriteMeshShaderFromFilter(shader, render_info->filter.get(), count);
    std::string cache_id = GetSpriteMeshShaderIdFromRenderInfo(render_info, count);
    shader.cache_id = cache_id;
    return cache_id;
}

SpriteItemRenderInfo* SpriteMesh::GetImageItemRenderInfo(SpriteItem* item) {
    auto& renderer = item->renderer_;
    BLEND_MODE_TYPE blending_cache = renderer->blending;
    std::string cache_prefix = item->cache_prefix_.empty() ? "0" : item->cache_prefix_;
    SpriteItemRenderInfo* ret = new SpriteItemRenderInfo;
    
    std::string filter_id = "$F$";
    if (item->GetFilter()) {
        if (!item->GetFilter()->GetShaderCacheId().empty()) {
            filter_id = item->GetFilter()->GetShaderCacheId();
        }
    }
    
    ret->side = renderer->side;
    ret->occlusion = renderer->occlusion;
    ret->blending = renderer->blending;
    ret->mask = renderer->mask;
    ret->mask_mode = renderer->mask_mode;
    ret->premultiply_alpha = renderer->texture->GetInnerTexture()->GetPremultiplyAlpha();
    ret->cache_prefix = cache_prefix;
    ret->filter = item->GetFilter();
    
    char* temp = new char[256];
    sprintf(temp, "%s.%s.%ld+%d+%ld+%f+%ld.%d.%d",
            cache_prefix.c_str(), filter_id.c_str(), (long) renderer->side, renderer->occlusion, (long) blending_cache,
            renderer->order, (long) renderer->mask_mode, renderer->mask, (int)ret->premultiply_alpha);
    ret->cache_id = temp;
    delete[] temp;
    
    return ret;
}

//std::string SpriteMesh::GetSpriteMeshShaderFromRenderInfo(Shader* shader, std::vector<std::string>& macros, SpriteItemRenderInfo* render_info, int count) {
//    std::stringstream temp;
//    if (!count) {
//        count = 2;
//    }
//    temp << "#define MAX_ITEM_COUNT " << count;
//    macros.push_back(temp.str());
//    if (render_info->premultiply_alpha) {
//        macros.push_back("#define PRE_MULTIPLY_ALPHA true");
//    }
//
//    shader->fragment = SPRITE_FRAGMENT_SOURCE;
//    shader->vertex = SPRITE_VERTEX_SOURCE;
//    shader->shaderd = true;
//
//    return GetSpriteMeshShaderIdFromRenderInfo(render_info, count);
//}

int SpriteMesh::GetMaxSpriteMeshItemCount() {
    return s_max_sprite_mesh_item_count_;
}

std::string SpriteMesh::GetSpriteMeshShaderIdFromRenderInfo(SpriteItemRenderInfo* render_info, int count) {
    char* temp = new char[256];
    if (render_info->filter) {
        std::string filter_cache_id = render_info->filter->GetMeshFilterOptions().shader_cache_id;
        sprintf(temp, "%s_mars_filter", filter_cache_id.c_str());
    } else {
        sprintf(temp, "%s_mars_sprite_%d_%d", render_info->cache_prefix.c_str(), count, (int) render_info->premultiply_alpha);
    }
    
    std::string ret = temp;
    delete[] temp;
    return ret;
}

SpriteMesh::SpriteMesh(SpriteItemRenderInfo* render_info, MarsRenderer* renderer, CalculateGroup* calculate_group) : calculate_group_(calculate_group){
    
    std::string name = "MSprite";
    name.append(std::to_string(s_seed++));
    mesh_ = std::make_shared<MarsMesh>(renderer, name);
    
    std::shared_ptr<MarsGeometry> geometry = std::make_shared<MarsGeometry>(renderer);
    AttributeWithData attr_point(4, SPRITE_VERTEX_STRIDE * sizeof(float), 0);
    geometry->SetAttributeInfo("aPoint", attr_point);
    AttributeWithData attr_index(2, SPRITE_VERTEX_STRIDE * sizeof(float), 4 * sizeof(float), "aPoint");
    geometry->SetAttributeInfo("aIndex", attr_index);
    std::shared_ptr<GPUBufferOption> index_buffer = std::make_shared<GPUBufferOption>(UNSIGNED_SHORT);
    geometry->SetIndexInfo(index_buffer);
    mesh_->SetGeometry(geometry);

    CreateMaterial(render_info, 2, mesh_);    
    is_dirty_ = false;
    max_item_count_ = GetMaxSpriteMeshItemCount();
}

SpriteMesh::~SpriteMesh() {
    DEBUG_MLOGD("SpriteMesh Destructor");
}

void SpriteMesh::CreateMaterial(SpriteItemRenderInfo* render_info, int count, std::shared_ptr<MarsMesh> mesh) {
    std::shared_ptr<MaterialRenderStates> states = std::make_shared<MaterialRenderStates>();
    switch (render_info->side) {
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
    states->depth_test = std::make_shared<MBool>(true);
    states->depth_mask = std::make_shared<MBool>(render_info->occlusion);
    
    MaterialOptions opt(states);
    opt.shader_cached_id = SpriteMeshShaderFromRenderInfo(opt.shader, render_info, count);
    pre_multi_alpha_ = MaterialUtil::SetMtlBlending(render_info->blending, *opt.states.get());
    MaterialUtil::SetMtlStencil(render_info->mask_mode, render_info->mask, *opt.states.get());
    mesh->SetMaterial(opt);
    auto material = mesh->GetMaterial();
    
    material->SetUniformValue("uViewProjection", std::make_shared<UniformValue>(FLOAT_MAT4, true, "uViewProjection"));
    material->SetUniformValue("uView", std::make_shared<UniformValue>(FLOAT_MAT4, true, "uView"));
    material->SetUniformValue("uModel", std::make_shared<UniformValue>(FLOAT_MAT4, true, "uModel"));
    material->SetUniformValue("uSamplerPre", std::make_shared<UniformValue>(SAMPLER_2D, true, SEMANTIC_MAIN_PRE_COLOR_ATTACHMENT_0));
    
    material->SetUniformValue("uPos", std::make_shared<UniformValue>(FLOAT_VEC4, (size_t) count));
    material->SetUniformValue("uSize", std::make_shared<UniformValue>(FLOAT_VEC2, (size_t) count));
    material->SetUniformValue("uColor", std::make_shared<UniformValue>(FLOAT_VEC4, (size_t) count));
    material->SetUniformValue("uTexParams", std::make_shared<UniformValue>(FLOAT_VEC4, (size_t) count));
    material->SetUniformValue("uTexOffset", std::make_shared<UniformValue>(FLOAT_VEC4, (size_t) count));
    material->SetUniformValue("uQuat", std::make_shared<UniformValue>(FLOAT_VEC4, (size_t) count));
    auto u_filter_blend_uniform = std::make_shared<UniformValue>(FLOAT_VEC2, (size_t) count);
    
    float blend_data[2] = { 1, 0 };
    u_filter_blend_uniform->SetData(blend_data, 0, 2);
    material->SetUniformValue("uFilterBlend", u_filter_blend_uniform);
    
    if (render_info->filter) {
        const MeshFilterOptions& mesh_filter_option = render_info->filter->GetMeshFilterOptions();
        const auto& filter_uniform_values = mesh_filter_option.uniform_values;
        for (auto iter : filter_uniform_values) {
            MLOGD("SetFilterMesh UniformValue: %s", iter.first.c_str());
            material->SetUniformValue(iter.first, iter.second);
        }
        material->states_->AssignState(mesh_filter_option.material_state);
    }
        
    mtl_slot_count_ = count;
}

void SpriteMesh::SetItems(std::vector<SpriteItem*> items) {
    int a_point_len = 0;
    int index_len = 0;
    int point_count = 0;
    if (!items.size()) {
        return;
    }
    std::vector<std::shared_ptr<SpriteMeshGeometry>> datas;
    items_.clear();
    for (auto it : items) {
        items_.push_back(it);
    }
    std::vector<std::shared_ptr<MarsTexture>> textures;
    int item_slot = 2;
    if (items.size() > 8) {
        item_slot = 16;
    } else if (items.size() > 4) {
        item_slot = 8;
    } else if (items.size() > 2) {
        item_slot = 4;
    }
    if (mtl_slot_count_ != item_slot) {
        // todo: { textures: false } ?
        CreateMaterial(items[0]->render_info_, item_slot, mesh_);
    }
    
    if (items[0]->GetFilter()) {
        // todo: refactor passSplitOption;
        split_layer_ = true;
    }
    
    for (int i = 0; i < items.size(); i++) {
        auto item = items[i];
        std::shared_ptr<MarsTexture> texture;
        if (item) {
            texture = item->renderer_->texture;
        }
        if (texture) {
            Utils::VectorAdd(textures, texture);
        }
    }
    
    for (int i = 0; i < items.size(); i++) {
        auto item = items[i];
        std::shared_ptr<MarsTexture> texture;
        if (item) {
            texture = item->renderer_->texture;
        }
        int texture_index = texture ? Utils::GetVectorIndexOfItem(textures, texture) : -1;
        auto data = GetItemInitData(item, i, point_count, texture_index);
        a_point_len += data->a_point_.size();
        index_len += data->index_.size();
        datas.push_back(data);
        point_count += data->a_point_.size() / 6;
        UpdateItem(item, true);
    }
    
    float* bundle_a_point = (float*) malloc(a_point_len * sizeof(float));
    memset(bundle_a_point, 0, a_point_len * sizeof(float));
    
    uint16_t* bundle_index = (uint16_t*) malloc(index_len * sizeof(uint16_t));
    memset(bundle_index, 0, index_len * sizeof(uint16_t));
    
    int cursor_a_point = 0;
    int cursor_index = 0;
    
    for (int i = 0; i < datas.size(); i++) {
        auto& data = datas[i];
        
        {
            auto& ta = data->a_point_;
            for (int j = 0; j < ta.size(); j++) {
                bundle_a_point[cursor_a_point + j] = ta[j];
            }
            cursor_a_point += ta.size();
        }
        
        {
            auto& ta = data->index_;
            for (int j = 0; j < ta.size(); j++) {
                bundle_index[cursor_index + j] = ta[j];
            }
            cursor_index += ta.size();
        }
    }
    
    auto& mtl = mesh_->material_;
    auto& geometry = mesh_->GetGeometries()[0];
    auto index_data = bundle_index;
    geometry->SetIndexData((uint8_t*) index_data, index_len * sizeof(uint16_t));
    geometry->SetAttributeData("aPoint", (uint8_t*) bundle_a_point, a_point_len * sizeof(float));
    geometry->SetDrawCount(index_len);
    mesh_->hide_ = geometry->GetDrawCount() > 0;
    mesh_->priority_ = items[0]->list_index_;
    for (int i = 0; i < textures.size(); i++) {
        std::shared_ptr<MarsTexture> texture = textures[i];
        std::string name = "uSampler";
        name.append(std::to_string(i));
        auto value = std::make_shared<UniformValue>(SAMPLER_2D);
        value->SetTexture(texture);
        mtl->SetUniformValue(name, value);
    }
    for (auto k = textures.size(); k < GetMaxSpriteMeshItemCount(); k++) {
        std::string name = "uSampler";
        name.append(std::to_string(k));
        if (mtl->GetUniformValue(name)) {
            mtl->SetUniformValue(name, nullptr);
        }
    }
    
    free(bundle_a_point);
    free(bundle_index);
}

void SpriteMesh::UpdateItem(SpriteItem* item, bool init) {
    int index = Utils::GetVectorIndexOfItem(items_, item);
    if (index > -1) {
        auto mtl = mesh_->material_;
        float* tex_data = (float*) mtl->GetUniformValue("uTexParams")->GetData();
        size_t idx_start = index * 4;
        
        SpriteRenderData* parent_data = nullptr;
        if (this->calculate_group_) {
            parent_data = this->calculate_group_->GetRenderData(item->parent_id_);
        }
        
        SpriteRenderData* self_data = item->GetRenderData(item->time_, init || (parent_data != nullptr));
        if (self_data->hide) {
            MN_SAFE_DELETE(self_data);
            return;
        }
        if (self_data->geo_data) {
            const auto geometry = mesh_->geometries_[0];
            geometry->SetIndexData((uint8_t*) self_data->geo_data->index_.data(), self_data->geo_data->index_.size() * sizeof(uint16_t));
            geometry->SetAttributeData("aPoint", (uint8_t*) self_data->geo_data->a_point_.data(), self_data->geo_data->a_point_.size() * sizeof(float));
        }
        Vec3 scale;
        size_t byte_start = idx_start * sizeof(float);
        {
            Vec3 position;
            Vec4 quat;
            self_data->transform->GetWorldTRS(&position, &quat, &scale);
            memcpy(mtl->GetUniformValue("uPos")->GetData() + byte_start, position.m, 3 * sizeof(float));
            ((float*) mtl->GetUniformValue("uPos")->GetData())[idx_start + 3] = 0;
            memcpy(mtl->GetUniformValue("uQuat")->GetData() + byte_start, quat.m, 4 * sizeof(float));
        }
        
        Vec4 color;
        float* color_data = (float *)(mtl->GetUniformValue("uColor")->GetData() + byte_start);
        color.Set(color_data[0], color_data[1], color_data[2], color_data[3]);
        
        MathUtil::Vec4MulCombine(color, self_data->color, parent_data ? parent_data->color : nullptr);
        memcpy(mtl->GetUniformValue("uColor")->GetData() + byte_start, color.m, 4 * sizeof(float));

        size_t idx = index * 2;
        float* s = (float*) mtl->GetUniformValue("uSize")->GetData();
        s[idx] = self_data->start_size->m[0] * scale.m[0];
        s[idx + 1] = self_data->start_size->m[1] * scale.m[1];
        auto renderer = item->renderer_;
        tex_data[idx_start] = renderer->occlusion ? (renderer->transparent_occlusion ? 1 : 0) : 1;
        tex_data[idx_start + 2] = renderer->render_mode;
        tex_data[idx_start + 3] = self_data->life;
        tex_data[idx_start + 1] = pre_multi_alpha_;
        if (self_data->tex_offset) {
            memcpy(mtl->GetUniformValue("uTexOffset")->GetData() + index * 4 * sizeof(float), self_data->tex_offset->m, 4 * sizeof(float));
        }
        MN_SAFE_DELETE(self_data);
    }
}

void SpriteMesh::ApplyChange() {
    if (is_dirty_) {
        SetItems(items_);
        is_dirty_ = false;
    }
}

std::shared_ptr<SpriteMeshGeometry> SpriteMesh::GetItemInitData(SpriteItem* item, int idx, int point_start_index, int texture_index) {
    auto geo_data = item->inner_geo_data_;
    if (!geo_data) {
        item->inner_geo_data_ = GetItemGeometryData(item, idx);
        geo_data = item->inner_geo_data_;
    }
    auto& point_data = geo_data->a_point_;
    if (point_data[4] != idx || point_data[5] != texture_index) {
        for (int i = 0; i < point_data.size(); i += 6) {
            point_data[i + 4] = idx;
            point_data[i + 5] = texture_index;
        }
    }
    auto& index = geo_data->index_;
    auto idx_count = index.size();
    
    if (item->custom_motph_) {
        return item->custom_motph_->Generate(0);
    } else {
        std::shared_ptr<SpriteMeshGeometry> ret = std::make_shared<SpriteMeshGeometry>();
        ret->a_point_ = point_data;
        for (int i = 0; i < idx_count; i++) {
            ret->index_.push_back(point_start_index + index[i]);
        }
        return ret;
    }
}

SpriteMeshGeometry* SpriteMesh::GetItemGeometryData(SpriteItem* item, int a_index) {
    SpriteMeshGeometry* ret = new SpriteMeshGeometry();
    
    auto& splits = item->splits_;
    auto& renderer = item->renderer_;
    auto& texture_sheet_animation = item->texture_sheet_animation_;
    item->FallbackAnchor();
    float anchor_x = 0, anchor_y = 0;
    item->GetAnchorXY(anchor_x, anchor_y);
    if (renderer->shape_ref) {
        ret->Copy(renderer->shape_ref);
        return ret;
    }
    float origin_data[8] = {0};
    Translate::ParticleOriginMap(origin_data, PARTICLE_ORIGIN_CENTER);
    float col = 2, row = 2;
    float dx = 0.0, dy = 0.0;
    if (splits.size() == 1) {
        col = 1;
        row = 1;
    }
    
    for (int x = 0; x < col; x++) {
        for (int y = 0; y < row; y++) {
            int index = y * 2 + x;
            std::vector<float> split = splits[index];
            if (texture_sheet_animation) {
                split = {0.0, 0.0, 1.0, 1.0, splits[0][4]};
            }
            float tex_offset[8] = { 0.0 };
            if (split[4]) {
                tex_offset[0] = 0.0;
                tex_offset[1] = 0.0;
                tex_offset[2] = 1.0;
                tex_offset[3] = 0.0;
                tex_offset[4] = 0.0;
                tex_offset[5] = 1.0;
                tex_offset[6] = 1.0;
                tex_offset[7] = 1.0;
            } else {
                tex_offset[0] = 0.0;
                tex_offset[1] = 1.0;
                tex_offset[2] = 0.0;
                tex_offset[3] = 0.0;
                tex_offset[4] = 1.0;
                tex_offset[5] = 1.0;
                tex_offset[6] = 1.0;
                tex_offset[7] = 0.0;
            }
            float dw = ((x + x + 1) / col - 1) / 2.0f;
            float dh = ((y + y + 1) / row - 1) / 2.0f;
            float tox = split[0];
            float toy = split[1];
            float tsx = split[4] ? split[3] : split[2];
            float tsy = split[4] ? split[2] : split[3];
            float origin[8] = { 0.0 };
            origin[0] = (origin_data[ 0 ] + dx) / col + dw;
            origin[1] = (origin_data[ 1 ] + dy) / row + dh;
            origin[2] = (origin_data[ 2 ] + dx) / col + dw;
            origin[3] = (origin_data[ 3 ] + dy) / row + dh;
            origin[4] = (origin_data[ 4 ] + dx) / col + dw;
            origin[5] = (origin_data[ 5 ] + dy) / row + dh;
            origin[6] = (origin_data[ 6 ] + dx) / col + dw;
            origin[7] = (origin_data[ 7 ] + dy) / row + dh;
            for (int i = 0; i < 4; i++) {
                ret->a_point_.push_back(origin[i * 2] - anchor_x);
                ret->a_point_.push_back(origin[i * 2 + 1] - anchor_y);
                ret->a_point_.push_back(tex_offset[i * 2] * tsx + tox);
                ret->a_point_.push_back(tex_offset[i * 2 + 1] * tsy + toy);
                ret->a_point_.push_back(a_index);
                ret->a_point_.push_back(0);
            }
            int base = index * 4;
            ret->index_.push_back(base);
            ret->index_.push_back(1 + base);
            ret->index_.push_back(2 + base);
            ret->index_.push_back(2 + base);
            ret->index_.push_back(1 + base);
            ret->index_.push_back(3 + base);
        }
    }
    
    return ret;
}

bool SpriteMesh::GetItemRenderData(SpriteVFXItemRenderData& ret, SpriteItem* item) {
    if (GetItemRegionData(ret, item)) {
        int index = Utils::GetVectorIndexOfItem(items_, item);
        const int idx = index * 4;
        const auto& mtl = mesh_->material_;
        const auto& r_data = mtl->GetUniformValue("uQuat");
        const auto& c_data = mtl->GetUniformValue("uColor");
        {
            float* r = (float*) r_data->GetData();
            ret.quat.Set(r[idx], r[idx + 1], r[idx + 2], r[idx + 3]);
        }
        {
            float* c = (float*) c_data->GetData();
            ret.color.Set(c[idx], c[idx + 1], c[idx + 2], c[idx + 3]);
        }
        return true;
    }
    return false;
}

bool SpriteMesh::GetItemRegionData(SpriteVFXItemRenderData& ret, SpriteItem* item) {
    int index = Utils::GetVectorIndexOfItem(items_, item);
    if (index > -1) {
        const auto& mtl = mesh_->material_;
        const auto& pos_data = mtl->GetUniformValue("uPos");
        const int idx = index * 4;
        if (pos_data && pos_data->byte_length > (idx + 2) * sizeof(float)) {
            float* temp = (float*) pos_data->GetData();
            ret.position.Set(temp[idx], temp[idx + 1], temp[idx + 2]);
        }
        const auto& r_data = mtl->GetUniformValue("uSize");
        {
            float* temp = (float*) r_data->GetData();
            ret.size.Set(temp[index * 2], temp[index * 2 + 1]);
        }
        return true;
    }
    return false;
}

}
