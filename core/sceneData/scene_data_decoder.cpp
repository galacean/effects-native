#include "scene_data_decoder.h"
#include "util/log_util.hpp"
#include "util/image_loader.h"
#include "util/constant.hpp"
#include "math/translate.h"

#define VERSION 2

namespace mn {

static ItemData* GetItemByName(CompositionData* comp, const std::string& name) {
    if (!comp) {
        MLOGE("SceneDataDecoder::GetItemByName composion is null");
        return nullptr;
    }
    ItemData* ret = nullptr;
    for (const auto& item : comp->items) {
        if (item->name.compare(name) == 0) {
            ret = item;
            break;
        }
    }
    return ret;
}

static bool SetupFangaoReplaceAndFollow(CompositionData* composition, FangaoData* fangao) {
    float camera_pos_z = 8;
    float camera_fov = 60;
    float global_width = composition->preview_size[0];
    float global_height = composition->preview_size[1];
    CameraData* camera = composition->camera;
    if (camera) {
        if (camera->position) {
            camera_pos_z = camera->position[2];
        }
        camera_fov = camera->fov;
    }
    float ratio = 2.0f / global_height * camera_pos_z * tan(camera_fov / 2 * M_PI / 180) + 1e-6;
    auto& replace = fangao->replace;
    for (int i = 0; i < replace.size(); i++) {
        auto& r = replace[i];
        const auto& name = r.name;
        ItemData* item = GetItemByName(composition, name);
        if (!item) {
            MLOGE("fangao replace item %s not found", name.c_str());
            return false;
        }
        if (item->type != ITEM_CONTENT_TYPE_SPRITE) {
            MLOGE("fangao replace item invalid type %d", item->type);
            return false;
        }
        if (!item->transform || !item->transform->scale || !item->transform->position) {
            MLOGE("fangao replace item no transform scale or position");
            return false;
        }
        float w0 = item->transform->scale[0];
        float h0 = item->transform->scale[1];
        float x0 = item->transform->position[0];
        float y0 = item->transform->position[1];
        float w1 = r.w * ratio;
        float h1 = r.h * ratio;
        float x1 = (r.x - global_width * 0.5f) * ratio;
        float y1 = (r.y - global_height * 0.5f) * ratio;
        item->transform->scale[0] = w1;
        item->transform->scale[1] = h1;
        float offset_x = 0;
        float offset_y = 0;
        if (item->content && item->content->renderer) {
            if (item->content->renderer->particle_origin) {
                PARTICLE_ORIGIN_TYPE particle_origin = CreateParticleOriginType(item->content->renderer->particle_origin->val);
                Translate::ParticleOriginPointData(offset_x, offset_y, particle_origin);
            }
            if (item->content->renderer->anchor) {
                auto& anchor = item->content->renderer->anchor;
                offset_x = 0.5f - anchor->m[0];
                offset_y = anchor->m[1] - 0.5f;
            }
        }
        x0 = x0 - (0.5f - offset_x) * w0;
        y0 = -y0 - (0.5f - offset_y) * h0;
        item->transform->position[0] = x1 + (0.5f - offset_x) * w1;
        item->transform->position[1] = -(y1 + (0.5f - offset_y) * h1);
        r.offset_l = x1 - x0;
        r.offset_r = (x1 + w1) - (x0 + w0);
        r.offset_t = y1 - y0;
        r.offset_b = (y1 + h1) - (y0 + h0);
        MLOGD("replace item(%s) %.2f %.2f %.2f %.2f -> %.2f %.2f %.2f %.2f", name.c_str(), x0, y0, w0, h0, x1, y1, w1, h1);
    }
    const auto& follow = fangao->follow;
    for (int i = 0; i < follow.size(); i++) {
        const auto& f = follow[i];
        ItemData* item = GetItemByName(composition, f.name);
        if (!item) {
            MLOGE("fangao follow item %s not found", f.name.c_str());
            return false;
        }
        if (item->type != ITEM_CONTENT_TYPE_SPRITE) {
            MLOGE("fangao follow item invalid type %d", item->type);
            return false;
        }
        if (!item->transform || !item->transform->position) {
            MLOGE("fangao follow item no transform position");
            return false;
        }
        for (int j = 0; j < replace.size(); j++) {
            if (replace[j].name.compare(f.auto_parent) == 0) {
                const auto& follow_type = f.auto_type;
                if (follow_type.find("L") != std::string::npos) {
                    item->transform->position[0] += replace[j].offset_l;
                } else if (follow_type.find("R") != std::string::npos) {
                    item->transform->position[0] += replace[j].offset_r;
                }
                if (follow_type.find("T") != std::string::npos) {
                    item->transform->position[1] -= replace[j].offset_t;
                } else if (follow_type.find("B") != std::string::npos) {
                    item->transform->position[1] -= replace[j].offset_b;
                }
                break;
            }
        }
    }
    return true;
}

SceneDataDecoder::~SceneDataDecoder() {
    MLOGD("SceneDataDecoder Destructor");
    if (buffer_) {
        delete buffer_;
    }
    ReleaseImageData();
    for (int i = 0; i < compositions_.size(); i++) {
        delete compositions_[i];
    }
    for (int i = 0; i < shapes_.size(); i++) {
        delete shapes_[i];
    }
    MN_SAFE_DELETE(fangao_);
}

void SceneDataDecoder::ReleaseImageData() {
    for (ImageRawData* data : images_datas_) {
        delete data;
    }
    images_datas_.clear();
}

void SceneDataDecoder::SetImageData(const std::string& image_url, ImageRawData* image_data) {
    int idx = -1;
    {
        // 通过image_url找到image_id
        auto iter = image_url_idx_map_.find(image_url);
        if (iter == image_url_idx_map_.end()) {
            MLOGE("SetImageRawData invalid url %s", image_url.c_str());
            return;
        }
        idx = iter->second;
    }
    if (idx >= 0) {
        // 通过image_id找到模版数据
        auto iter = temp_image_info_.find(idx);
        if (iter != temp_image_info_.end()) {
            auto& info = iter->second;
            // 如果是变量图片，覆盖旧的数据
            ImageRawData* old_data = images_datas_[info.idx];
            if (old_data) {
                delete old_data;
            }
            MLOGD("SetImageRawData template :%s %d %p", image_url.data(), info.idx, image_data);
            images_datas_[info.idx] = image_data;
            return;
        }
        // 如果当前的图片没有设置过，直接塞进去，防止覆盖模版塞入的数据
        if (!images_datas_[idx]) {
            MLOGD("SetImageRawData url is :%s %d %p", image_url.data(), idx, image_data);
            images_datas_[idx] = image_data;
            return;
        }
        // 直接销毁没用的图片
        delete image_data;
    }
}

// 通过image_url查找是否存在动参模版
void SceneDataDecoder::GetImageExpectedSize(int& width, int& height, const std::string& image_url) {
    int idx = -1;
    width = 0;
    height = 0;
    {
        // 通过image_url找到image_id
        auto iter = image_url_idx_map_.find(image_url);
        if (iter == image_url_idx_map_.end()) {
            MLOGE("SetImageRawData invalid url %s", image_url.c_str());
            return;
        }
        idx = iter->second;
    }
    if (idx >= 0) {
        // 通过image_id找到模版数据
        auto iter = temp_image_info_.find(idx);
        if (iter != temp_image_info_.end()) {
            auto& info = iter->second;
            width = info.expected_width;
            height = info.expected_height;
        }
    }
}

void SceneDataDecoder::SetAnimationLoop(bool looping) {
    if (compositions_[0]) {
        auto composition = compositions_[0];
        MN_SAFE_DELETE(composition->end_behavior)
        if (looping) {
            composition->end_behavior = new MInt(END_BEHAVIOR_RESTART);
        } else {
            composition->end_behavior = new MInt(END_BEHAVIOR_PAUSE);
        }
    }
}

bool SceneDataDecoder::InitComposition() {
    CompositionData* composition = nullptr;
    for (int i = 0; i < compositions_.size(); i++) {
        if (strcmp(compositions_[i]->id.c_str(), composition_id_) == 0) {
            composition = compositions_[i];
            break;
        }
    }
    if (!composition) {
        MLOGE("invalid composition id: %d", composition_id_);
        return false;
    }
    for (const auto& item : composition->items) {
        if (item->type < ITEM_CONTENT_TYPE_SPRITE || item->type > ITEM_CONTENT_TYPE_CALCULATE) {
            continue;
        }
        const auto& content = item->content;
        
        // todo: ui
        SpriteContentData* render_content = nullptr;
        if (item->type >= ITEM_CONTENT_TYPE_SPRITE || item->type <= ITEM_CONTENT_TYPE_CALCULATE) {
            render_content = (SpriteContentData*) content;
        }
        if (render_content) {
            if (render_content->renderer) {
                if (render_content->renderer->shape_id) {
                    const std::vector<float>* split = nullptr;
                    if (render_content->splits && render_content->splits->splits.size() > 0) {
                        split = &(render_content->splits->splits[0]);
                    }
                    render_content->renderer->shape =
                        SpriteMeshGeometry::GetGeometryByShape(shapes_[render_content->renderer->shape_id->val], split);
                }
            } else {
                render_content->renderer = new ItemRendererOptionsData;
                render_content->renderer->order = new MFloat(0);
            }
            // todo: trails
            // todo: filter
        }
    }
    
    if (fangao_ && !SetupFangaoReplaceAndFollow(composition, fangao_)) {
        return false;
    }
    return true;
}

CompositionData* SceneDataDecoder::GetComposition() {
    CompositionData* composition = nullptr;
    for (int i = 0; i < compositions_.size(); i++) {
        if (strcmp(compositions_[i]->id.c_str(), composition_id_) == 0) {
            composition = compositions_[i];
            break;
        }
    }
    if (!composition) {
        MLOGE("invalid composition id: %d", composition_id_);
        return nullptr;
    }
    return composition;
}

}
