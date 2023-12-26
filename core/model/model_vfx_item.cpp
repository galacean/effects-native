//
//  model_vfx_item.cpp
//
//  Created by Zongming Liu on 2022/9/16.
//

#include "model_vfx_item.hpp"
#include "renderer/m_entity.hpp"
#include "renderer/m_mesh.hpp"
#include "sceneData/data/model_data.h"
#include "player/composition.h"

namespace mn {

static void SyncMarsItemModelBase(std::shared_ptr<MarsItemModelBase> options, ItemData* arg) {
    options->id = arg->id;
    options->type = arg->type;
    options->parent_id = arg->parent_id;
    options->delay = arg->delay;
    options->name = arg->name;
    options->duration = arg->duration;
    options->endBehavior = arg->end_behavior ? arg->end_behavior->val : 0;
    options->render_level = arg->render_level;
    if (arg->transform) {
        auto t = arg->transform;
        if (t->position) {
            auto p = t->position;
            options->transform.position.Set(p[0], p[1], p[2]);
        }
        if (t->rotation) {
            auto r = t->rotation;
            options->transform.rotation.Set(r[0], r[1], r[2]);
        }
        if (t->scale) {
            auto s = t->scale;
            options->transform.scale.Set(s[0], s[1], s[2]);
        }
    }
}

ModelVFXItem::ModelVFXItem() : VFXItem() {
}

void ModelVFXItem::OnConstructed(ItemData* arg) {
    model_content_ref_ = arg->content;
    if (arg->type == ITEM_CONTENT_TYPE_MODEL_MESH) {
        std::shared_ptr<MarsItemMesh> opt = std::make_shared<MarsItemMesh>();
        ModelMeshContentData* mesh_content = (ModelMeshContentData*) arg->content;
        if (mesh_content->options) {
            opt->options = std::make_shared<MarsMeshOptions>();
            opt->options->hide = false;
            opt->options->parent = mesh_content->options->parent ? mesh_content->options->parent->val : -1;
            if (mesh_content->options->skin) {
                opt->options->skin = std::make_shared<MarsSkinOptions>();
                mesh_content->options->skin->CopyTo(opt->options->skin);
            }
            for (int i = 0; i < mesh_content->options->primitives.size(); i++) {
                std::shared_ptr<MarsPrimitiveOptions> pri = std::make_shared<MarsPrimitiveOptions>();
                mesh_content->options->primitives[i]->CopyTo(pri, composition_);
                opt->options->primitives.push_back(pri);
            }
        }
        options_ = opt;
    } else if (arg->type == ITEM_CONTENT_TYPE_MODEL_SKYBOX) {
        std::shared_ptr<MarsItemSkybox> opt = std::make_shared<MarsItemSkybox>();
        SkyboxContentData* sky_content = (SkyboxContentData*) arg->content;
        
        if (sky_content->options) {
            opt->options = std::make_shared<MarsSkyboxOptions>();
            opt->options->intensity = sky_content->options->intensity;
            opt->options->reflection_intensity = sky_content->options->reflections_intensity;
            opt->options->irradiance_coeffs = sky_content->options->irradianceCoeffs;
            opt->options->specular_mip_count = sky_content->options->specular_mip_count;
            opt->options->specular_image_size = sky_content->options->specular_image_size;
            
            if (sky_content->options->diffuse_image) {
                opt->options->diffuse_image = sky_content->options->diffuse_image->ToMarsTexture(composition_);
            }
            
            if (sky_content->options->specular_image) {
                opt->options->specular_image = sky_content->options->specular_image->ToMarsTexture(composition_);
            }
        } else {
            MLOGE("decode data failed");
            assert(0);
        }
        
        options_ = opt;
    } else if (arg->type == ITEM_CONTENT_TYPE_MODEL_LIGHT) {
        std::shared_ptr<MarsItemLight> opt = std::make_shared<MarsItemLight>();
        ModelLightContentData* light_content = (ModelLightContentData*) arg->content;
        if (light_content->options) {
            opt->options = std::make_shared<MarsLightOption>();
            opt->options->type = light_content->options->light_type;
            opt->options->range = light_content->options->range;
            size_t color_len = light_content->options->color.size();
            for (size_t i=0; i<color_len; i++) {
                opt->options->color.m[i] = light_content->options->color[i];
            }
            opt->options->intensity = light_content->options->intensity;
            opt->options->inner_cone_angle = light_content->options->innerConeAngle;
            opt->options->outer_cone_angle = light_content->options->outerConeAngle;
        } else {
            MLOGE("decode light content failed");
            assert(0);
        }
        options_ = opt;
    } else if (arg->type == ITEM_CONTENT_TYPE_MODEL_CAMERA) {
        MLOGE("decode camera item failed");
        assert(0);
    } else {
        MLOGE("decode model item failed not support type");
        assert(0);
    }
    
    SyncMarsItemModelBase(options_, arg);
}

BaseContentItem* ModelVFXItem::InnerCreateContent() {
//    if (content_3d_ && content_3d_->GetType() == MEntityType::Mesh) {
//        assert(0);
//    }
    return nullptr;
}

void ModelVFXItem::OnLifeTimeBegin(Composition* renderer, void* content) {
    if (content_3d_) {
        if (content_3d_->GetType() == MEntityType::Mesh && parent && parent->GetType().compare("tree") == 0) {
            std::static_pointer_cast<MMesh>(content_3d_)->UpdateParentItem(std::static_pointer_cast<TreeVFXItem>(parent));
        }
        content_3d_->OnVisibleChange(true);
    }
}

void ModelVFXItem::OnItemUpdate(float dt, float lifetime) {
    if (content_3d_) {
        Mat4 parent_mat4;
        transform_->GetWorldMatrix(parent_mat4);
        content_3d_->Matrix().Set(parent_mat4);
        if (content_3d_->GetType() == MEntityType::Camera) {
            assert(0);
        }
    }
}

void ModelVFXItem::OnItemRemoved(Composition* renderer, void* content) {
    if (content_3d_) {
        content_3d_->OnEntityRemoved();
    }
}

}
