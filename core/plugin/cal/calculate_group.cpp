#include "calculate_group.h"
#include "util/log_util.hpp"
#include "math/math_util.hpp"

namespace mn {

struct TraverseOpt {
    bool stopped = false;
};

static void Traverse(std::vector<std::shared_ptr<CGNode>>& nodes, CalculateItem* content, TraverseOpt& opt) {
    for (auto iter = nodes.begin(); iter != nodes.end(); iter++) {
        const auto& node = *iter;
        if (node->content_ == content) {
            nodes.erase(iter);
            opt.stopped = true;
            return;
        }
        Traverse(node->children_, content, opt);
        if (opt.stopped) {
            return;
        }
    }
}

CGNode::CGNode(CalculateItem* content) {
    content_ = content;
    content_life_checker_ = content->life_checker_;
}

CGNode::~CGNode() {
}

void CGNode::RemoveContent(std::vector<std::shared_ptr<CGNode>>& nodes, CalculateItem* content) {
    TraverseOpt opt;
    Traverse(nodes, content, opt);
}

static void UpdateNode(std::shared_ptr<CGNode> node, CalculateGroup* self, float dt) {
    const auto& item = node->content_;
    if (node->content_life_checker_.lock()) {
        item->inner_transform_->Invalid();
        item->OnUpdate(dt, self);
    }
    if (node->children_.size()) {
        for (int i = 0; i < node->children_.size(); i++) {
            UpdateNode(node->children_[i], self, dt);
        }
    }
}

SpriteRenderData* CalculateGroup::CombineRenderData(SpriteRenderData* data, SpriteRenderData* parent_data) {
    if (parent_data) {
        data->transform->Update();
        if (!data->color) {
            data->color = new Vec4();
        }
        MathUtil::Vec4MulCombine(*data->color, data->color, parent_data->color);
        if (parent_data->hide) {
            data->hide = true;
        }
    }
    return data;
}

CalculateGroup::CalculateGroup() {
}

CalculateGroup::~CalculateGroup() {
    for (auto iter : item_map_) {
        iter.second->ReleaseRef();
    }
}

void CalculateGroup::OnUpdate(float dt) {
    time_ += dt / 1000;
    for (int i = 0; i < nodes_.size(); i++) {
        UpdateNode(nodes_[i], this, dt);
    }
}

SpriteRenderData* CalculateGroup::GetRenderData(std::string id) {
    auto iter = item_map_.find(id);
    if (iter != item_map_.end()) {
        return iter->second->render_data_;
    }
    return nullptr;
}

void CalculateGroup::AddItem(CalculateItem* item) {
    for (int i = 0; i < items_.size(); i++) {
        if (items_[i] == item) {
            return;
        }
    }
    items_.push_back(item);
}

std::string CalculateGroup::InnerRemoveItemRef(const std::string& id) {
    std::string ret;
    auto iter = item_map_.find(id);
    if (iter != item_map_.end()) {
        CalculateItem* item = iter->second;
        if (item) {
            int c = --item->ref_count_;
            if (c == 0) {
                item_map_.erase(iter);
                ret = item->parent_id_;
                item->ReleaseRef();
                CGNode::RemoveContent(nodes_, item);
                return ret;
            }
        } else {
            MLOGE("InnerRemoveItemRef failed item is null");
        }
    }
    return ret;
}

void CalculateGroup::RemoveItemRef(const std::string& id) {
    std::string parent_id = InnerRemoveItemRef(id);
    if (parent_id.size()) {
        InnerRemoveItemRef(parent_id);
    }
}

void CalculateGroup::BuildTree(const std::map<std::string, int>& ref_count_map) {
    std::map<std::string, std::shared_ptr<CGNode>> node_map;
    std::vector<std::shared_ptr<CGNode>> nodes;
    for (int i = 0; i < items_.size(); i++) {
        CalculateItem* c = items_[i];
        c->AddRef();
        item_map_[c->id_] = c;
        auto cn = std::make_shared<CGNode>(c);
        node_map[c->id_] = cn;
        nodes.push_back(cn);
    }
    nodes_.clear();
    for (int i = 0; i < nodes.size(); i++) {
        const auto& node = nodes[i];
        const auto& item = node->content_;
        if (item->parent_id_.size()) {
            auto iter = node_map.find(item->parent_id_);
            if (iter != node_map.end()) {
                iter->second->children_.push_back(node);
            } else {
                nodes_.push_back(node);
            }
        } else {
            nodes_.push_back(node);
        }
    }
    for (auto iter : node_map) {
        auto& node = iter.second;
        node->content_->ref_count_ = (int) (node->children_.size() + 1);
        auto iter1 = ref_count_map.find(node->content_->id_);
        if (iter1 != ref_count_map.end()) {
            node->content_->ref_count_ += iter1->second;
        }
    }
    items_.clear();
}

}
