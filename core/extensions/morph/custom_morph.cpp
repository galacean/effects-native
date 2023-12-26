//
//  custom_morph.cpp
//  MarsNative
//
//  Created by changxing on 2023/9/14.
//  Copyright © 2023 Alipay. All rights reserved.
//

#include "custom_morph.hpp"
#include "plugin/sprite/sprite_item.h"

namespace mn {

CustomMorph::CustomMorph(const std::string& name, const MN_CUSTOM_MORPH_CALLBACK& callback) {
    name_ = name;
    callback_ = callback;
    MLOGD("CustomMorph::alloc");
}

CustomMorph::~CustomMorph() {
    MLOGD("CustomMorph::dealloc");
}

std::shared_ptr<SpriteMeshGeometry> CustomMorph::Generate(float lifetime) {
    float* points = nullptr;
    int points_length = 0;
    uint16_t* indices = nullptr;
    int indices_length = 0;
    callback_(name_, lifetime, &points, &points_length, &indices, &indices_length);
    
    std::shared_ptr<SpriteMeshGeometry> ret = std::make_shared<SpriteMeshGeometry>();
    if (!points) {
        MLOGE("CustomMorph::Generate fail");
        return ret;
    }
    for (int i = 0; i < points_length; i++) {
        ret->a_point_.push_back(points[i]);
    }
    for (int i = 0; i < indices_length; i++) {
        ret->index_.push_back(indices[i]);
    }
    delete [] points;
    delete [] indices;
    return ret;
}

CustomMorphLoader::CustomMorphLoader(const std::vector<std::string>& params, const MN_CUSTOM_MORPH_CALLBACK& callback) {
    name_ = "morph";
    params_ = params;
    callback_ = callback;
    MLOGD("CustomMorphLoader::dealloc");
}

CustomMorphLoader::~CustomMorphLoader() {
    MLOGD("CustomMorphLoader::dealloc");
    if (callback_) {
        callback_(name_, 1, nullptr, nullptr, nullptr, nullptr);
        callback_ = NULL;
    }
}

void CustomMorphLoader::OnCompositionItemLifeBegin(Composition* comp, std::shared_ptr<VFXItem> item) {
    if (item->GetType().compare(VFX_ITEM_TYPE_SPRITE) == 0) {
        SpriteItem* content = (SpriteItem*) item->content_;
        for (int i = 0; i < params_.size(); i++) {
            if (content->name_.compare(params_[i]) == 0 && content->render_info_->mask_mode == MASK_MODE_WRITE_MASK) {
                content->SetCustomMorph(new CustomMorph(content->name_, callback_));
                break;
            }
        }
    }
}

}
