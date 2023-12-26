//
//  mars_ext_position_path_modifier.cpp
//  MarsNative
//
//  Created by changxing on 2023/9/12.
//  Copyright © 2023 Alipay. All rights reserved.
//

#include "mars_ext_position_path_modifier.hpp"
#include "sceneData/scene_data_decoder.h"

namespace mn {

MarsExtPositionPathModifier::MarsExtPositionPathModifier() {
}

MarsExtPositionPathModifier::~MarsExtPositionPathModifier() {
    // 清理路径数据缓存
    for (float* data : data_to_free_) {
        if (data) {
            free(data);
        }
    }
    data_to_free_.clear();
}

bool MarsExtPositionPathModifier::SetSceneData(SceneDataDecoder* scene_data) {
    if (!scene_data) {
        // 没有场景数据，退出
        MLOGE("MarsExtPositionPathModifier: no scene data");
        return false;
    }
    const auto& compositions = scene_data->compositions_;
    if (compositions.empty()) {
        // 没有合成，退出
        MLOGE("MarsExtPositionPathModifier: no composition");
        return false;
    }
    if (compositions.size() > 1) {
        // 有多个合成，退出
        MLOGE("MarsExtPositionPathModifier: multi compositions");
        return false;
    }
    composition_ = compositions[0];
    
    // 计算Mars坐标转换参数
    float camera_pos_z = 8;
    float camera_fov = 60;
    anim_width_ = compositions[0]->preview_size[0];
    anim_height_ = compositions[0]->preview_size[1];
    CameraData* camera = compositions[0]->camera;
    if (camera) {
        if (camera->position) {
            camera_pos_z = camera->position[2];
        }
        camera_fov = camera->fov;
    }
    pixel_ratio_ = 2.0f / anim_width_ * camera_pos_z * tan(camera_fov / 2 * M_PI / 180) + 1e-6;
    aspect_ = anim_width_ / anim_height_;
    
    return true;
}

bool MarsExtPositionPathModifier::GetPositionPathByName(const std::string& name, float** data, int* length, float** data2, int* length2) {
    // 数据较少，暂时先直接遍历
    const auto& items = composition_->items;
    for (ItemData* item : items) {
        if (item->name.compare(name) == 0) {
            // 匹配到name
            if ((item->type != ITEM_CONTENT_TYPE_SPRITE) && (item->type != ITEM_CONTENT_TYPE_CALCULATE)) {
                MLOGE("MarsExtPositionPathModifier: item(%s) invalid type", name.c_str());
                return false;
            }
            // 只有sprite和calculate节点支持
            SpriteContentData* content = (SpriteContentData*) item->content;
            if (!content) {
                // 没有content，退出
                MLOGE("MarsExtPositionPathModifier: item(%s)  no content", name.c_str());
                return false;
            }
            ItemPositionOverLifetimeData* position_over_lifetime = content->position_over_lifetime;
            if (!position_over_lifetime) {
                // 没有position_over_lifetime，退出
                MLOGE("MarsExtPositionPathModifier: item(%s) no position_over_lifetime", name.c_str());
                return false;
            }
            if (position_over_lifetime->path_value && (position_over_lifetime->path_value->type_.compare("bezier") == 0)) {
                // path_value暂时仅支持贝塞尔曲线
                MValue* path_value = position_over_lifetime->path_value;
                if (data) {
                    *data = path_value->data_;
                }
                if (length) {
                    *length = path_value->length_;
                }
                if (data2) {
                    *data2 = path_value->data2_;
                }
                if (length2) {
                    *length2 = path_value->length2_;
                }
                return true;
            }
            if (item->transform->position) {
                if (data) {
                    *data = item->transform->position;
                }
                if (length) {
                    *length = 3;
                }
                if (data2) {
                    *data2 = nullptr;
                }
                if (length2) {
                    *length2 = 0;
                }
                return true;
            }
            
            MLOGE("MarsExtPositionPathModifier: item(%s) invalid position path", name.c_str());
            return false;
        }
    }
    return false;
}

void MarsExtPositionPathModifier::ModifyPositionPath(const std::string& name, float* value, int length, float* value2, int length2) {
    data_to_free_.push_back(value);
    data_to_free_.push_back(value2);
    // 数据较少，暂时先直接遍历
    const auto& items = composition_->items;
    for (ItemData* item : items) {
        if (item->name.compare(name) == 0) {
            // 匹配到name
            if ((item->type != ITEM_CONTENT_TYPE_SPRITE) && (item->type != ITEM_CONTENT_TYPE_CALCULATE)) {
                MLOGE("ModifyPositionPath: item(%s) invalid type", name.c_str());
                return;
            }
            // 只有sprite和calculate节点支持
            SpriteContentData* content = (SpriteContentData*) item->content;
            if (!content) {
                // 没有content，退出
                MLOGE("ModifyPositionPath: item(%s)  no content", name.c_str());
                return;
            }
            if (value2) {
                ItemPositionOverLifetimeData* position_over_lifetime = content->position_over_lifetime;
                if (!position_over_lifetime) {
                    // 没有position_over_lifetime，退出
                    MLOGE("ModifyPositionPath: item(%s) no position_over_lifetime", name.c_str());
                    return;
                }
                if (!position_over_lifetime->path_value || (position_over_lifetime->path_value->type_.compare("bezier") != 0)) {
                    // 暂时仅支持贝塞尔曲线
                    MLOGE("ModifyPositionPath: item(%s) invalid position path", name.c_str());
                    return;
                }
                MValue* path_value = position_over_lifetime->path_value;
                path_value->data_ = value;
                path_value->length_ = length;
                path_value->data2_ = value2;
                path_value->length2_ = length2;
            } else {
                item->transform->position = value;
            }
            MLOGE("ModifyPositionPath: item(%s) success", name.c_str());
            return;
        }
    }
}

}
