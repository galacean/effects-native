//
//  filter_utils.cpp
//  MarsNative
//
//  Created by Zongming Liu on 2022/7/19.
//  Copyright © 2022 Alipay. All rights reserved.
//

#include "filter_utils.hpp"
#include "plugin/sprite/sprite_loader.h"
#include "filters/gaussian_filter_define.hpp"
#include "filters/bloom_filter_define.hpp"
#include "filters/delay_filter_define.hpp"
#include "filters/distortion_filter_define.hpp"

namespace mn {

std::shared_ptr<FilterDefine> FilterUtils::GenerateFilter(Composition* composition, ItemFilterData* filter_data) {
    if (filter_data->name == "gaussian") {
        return std::make_shared<GaussianFilterDefine>(composition, static_cast<GaussianFilterData *>(filter_data));
    } else if (filter_data->name == "bloom") {
        return std::make_shared<BloomFilterDefine>(composition, static_cast<BloomFilterData *>(filter_data));
    } else if (filter_data->name == "delay") {
        return std::make_shared<DelayFilterDefine>(composition, static_cast<DelayFilterData *>(filter_data));
    } else if (filter_data->name == "distortion") {
        return std::make_shared<DistortionFilterDefine>(composition, static_cast<DistortionFilterData *>(filter_data));
    } else {
        MLOGE("Not Support Filter %s", filter_data->name.c_str());
        return nullptr;
    }
}

std::shared_ptr<MarsMesh> FilterUtils::CloneSpriteMesh(std::shared_ptr<MarsMesh> clone_mesh, Composition* composition, FilterDefine* filter_define) {    
    auto is_sprite = [](MarsPlugin* plugin) { return (plugin->name_) == "SpriteLoader"; };
    const auto& plugins = composition->plugin_system_->loaders_;
    auto loader_iter = std::find_if(plugins.begin(), plugins.end(), is_sprite);
    
    if (loader_iter != std::end(plugins)) {
        SpriteLoader* sprite_loader = (SpriteLoader*) *loader_iter;
        Shader shader;
        sprite_loader->SpriteMeshShaderFromFilter(shader, filter_define);
        return CloneMeshWithShader(clone_mesh, composition, shader);
    }
    
    return nullptr;
}

std::shared_ptr<MarsMesh> FilterUtils::CloneMeshWithShader(std::shared_ptr<MarsMesh> clone_mesh, Composition* composition, const Shader& shader) {
    std::shared_ptr<MaterialRenderStates> states = std::make_shared<MaterialRenderStates>();
    states->blending = std::make_shared<MBool>(false);
    states->cull_face_enabled = std::make_shared<MBool>(false);
    states->depth_test = std::make_shared<MBool>(false);
    
    MaterialOptions material_option(states);
    material_option.shader = shader;
    
    auto copy_material = std::make_shared<MarsMaterial>(composition->renderer_, material_option);
    auto copy_geometry = clone_mesh->GetGeometry();
    std::shared_ptr<MarsMesh> mesh = std::make_shared<MarsMesh>(composition->renderer_);
    
    auto uniform_values = clone_mesh->GetMaterial()->GetUniformValues();
    for (auto& iter : uniform_values) {
        copy_material->SetUniformValue(iter.first, iter.second);
    }
    
    mesh->SetGeometry(copy_geometry);
    mesh->SetMaterial(copy_material);
    return mesh;
}

}
