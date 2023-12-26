//
//  tree_vfx_item.cpp
//
//  Created by Zongming Liu on 2022/10/13.
//

#include "tree_vfx_item.hpp"
#include "model/sceneData/data/model_data.h"

namespace mn {

static std::shared_ptr<MarsAnimTrackOptions> CreateMarsAnimTrackOptions(ModelAnimTrackOptionsData* data) {
    std::shared_ptr<MarsAnimTrackOptions> ret = std::make_shared<MarsAnimTrackOptions>();
    ret->node = data->node;
    ret->input = data->input;
    ret->input_byte_length = data->input_length;
    ret->output = data->output;
    ret->output_byte_length = data->output_length;
    ret->path = data->path;
    ret->interpolation = data->interpolation;
    return ret;
}

static std::shared_ptr<MarsAnimationOptions> CreateAnimationOptions(ModelAnimationOptionsData* data) {
    std::shared_ptr<MarsAnimationOptions> ret = std::make_shared<MarsAnimationOptions>();
    ret->name = data->name;
    for (int i = 0; i < data->tracks.size(); i++) {
        ret->tracks.push_back(CreateMarsAnimTrackOptions(data->tracks[i]));
    }
    return ret;
}

static std::shared_ptr<TreeNodeOptions> CreateTreeNodeOptions(TreeNodeOptionsData* data) {
    std::shared_ptr<TreeNodeOptions> ret = std::make_shared<TreeNodeOptions>();
    ret->name = data->name;
    if (data->transform) {
        ret->transform_data = new TransformData;
        if (data->transform->position) {
            float* p = data->transform->position;
            ret->transform_data->position_ = new Vec3(p[0], p[1], p[2]);
        }
        if (data->transform->scale) {
            float* s = data->transform->scale;
            ret->transform_data->scale_ = new Vec3(s[0], s[1], s[2]);
        }
        if (data->transform->quat) {
            float* q = data->transform->quat;
            ret->transform_data->quat_ = new Vec4(q[0], q[1], q[2], q[3]);
        }
        if (data->transform->rotation) {
            float* r = data->transform->rotation;
            ret->transform_data->SetRotationData(r[0], r[1], r[2]);
        }
    }
    for (int i = 0; i < data->children_length; i++) {
        ret->children.push_back(data->children[i]);
    }
    ret->id = data->id;
    return ret;
}

TreeVFXItem::TreeVFXItem() {
    
}

TreeVFXItem::~TreeVFXItem() {
    
}

void TreeVFXItem::OnConstructed(ItemData* arg) {
    options_ = std::make_shared<TreeItemOptions>();
    options_->tree = std::make_shared<TreeOptionsAnimEx>();
    ModelTreeItemOptionsData* options_data = ((ModelTreeContentData*) arg->content)->options;
    auto tree_data = options_data->tree;
    if (tree_data) {
        auto tree = options_->tree;
        tree->animation = tree_data->animation;
        for (int i = 0; i < tree_data->animations.size(); i++) {
            tree->animation_options.push_back(CreateAnimationOptions(tree_data->animations[i]));
        }
        for (int i = 0; i < tree_data->nodes.size(); i++) {
            tree->nodes.push_back(CreateTreeNodeOptions(tree_data->nodes[i]));
        }
        for (int i = 0; i < tree_data->children_length; i++) {
            tree->children.push_back(tree_data->children[i]);
        }
    }
}

BaseContentItem* TreeVFXItem::InnerCreateContent() {
    return new Tree(options_->tree, transform_);;
}

void TreeVFXItem::InnerOnUpdate(float dt, std::shared_ptr<VFXItem> thiz) {
}

void TreeVFXItem::OnUpdateByLoader(float dt, std::shared_ptr<VFXItem> thiz) {
    VFXItem::InnerOnUpdate(dt, thiz);
}

}
