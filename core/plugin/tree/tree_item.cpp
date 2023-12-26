//
//  tree_item.cpp
//
//  Created by Zongming Liu on 2022/10/13.
//

#include "tree_item.hpp"

namespace mn {

Tree::Tree(std::shared_ptr<TreeOptions> options, std::shared_ptr<Transform> root_transform) {
    DEBUG_MLOGD("Tree Construct");
    tree_transform_ = root_transform;
    this->Build(options);
}

Tree::~Tree() {
    DEBUG_MLOGD("Tree Destruct");
}

//std::shared_ptr<Transform> Tree::GetNodeTransform() {
//    return tree_transform_;
//}

std::shared_ptr<TreeNode> Tree::GetNodeById(const std::string& id) {
    auto search = cache_map_.find(id);
    if (search == cache_map_.end()) {
        std::string id_perfix = "^";
        id_perfix.append(id);
        for (size_t i=0; i<all_nodes_.size(); i++) {
            auto node = all_nodes_[i];
            if (node->id == id_perfix) {
                cache_map_.insert({id, node});
                return node;
            }
        }
        return nullptr;
    } else {
        return cache_map_[id];
    }
}

std::shared_ptr<Transform> Tree::GetNodeTransform(const std::string& id) {
    auto node = this->GetNodeById(id);
    if (node) {
        return node->transform;
    } else {
        return tree_transform_;
    }
}

void Tree::Build(std::shared_ptr<TreeOptions> options) {
    //    const Mat4& top_transform = tree_transform_;
    auto nodes = options->nodes;
    for (size_t i=0; i<nodes.size(); i++) {
        std::shared_ptr<TreeNode> tree_node = std::make_shared<TreeNode>();
        if (nodes[i]->name.empty()) {
            tree_node->name = nodes[i]->id;
        } else {
            tree_node->name = nodes[i]->name;
        }
        std::string id_perfix = "^";
        if (nodes[i]->id.empty()) {
            id_perfix.append(std::to_string(i));
        } else {
            id_perfix.append(nodes[i]->id);
        }
        tree_node->id = id_perfix;
        tree_node->tree = this;
        tree_node->transform = std::make_shared<Transform>(nodes[i]->transform_data);
        tree_node->transform->SetParentTransform(tree_transform_);
        all_nodes_.push_back(tree_node);
    }
    
    for (size_t i=0; i<all_nodes_.size(); i++) {
        std::vector<int> children_index = nodes[i]->children;
        if (children_index.size() > 0) {
            auto tree_node = all_nodes_[i];
            for (size_t j=0; j<children_index.size(); j++) {
                int child_idx = children_index[j];
                auto children_tree_node = all_nodes_.at(child_idx);
                if (children_tree_node && children_tree_node != tree_node) {
                    children_tree_node->transform->SetParentTransform(tree_node->transform);
                    tree_node->children.push_back(children_tree_node);
                }
            }
        }
    }
    
    auto children = options->children;
    for (size_t i=0; i<children.size(); i++) {
        nodes_.push_back(all_nodes_[i]);
    }
}

}
