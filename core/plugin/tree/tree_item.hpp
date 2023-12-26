//
//  tree_item.hpp
//
//  Created by Zongming Liu on 2022/10/13.
//

#ifndef tree_item_hpp
#define tree_item_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "math/transform.h"
#include "plugin/types.h"

namespace mn {

class Tree;

struct TreeNode {
    std::string name;
    std::shared_ptr<Transform> transform = std::make_shared<Transform>();
    std::vector<std::shared_ptr<TreeNode>> children;
    std::string id;
    Tree* tree = nullptr;
};

struct TreeNodeOptions {
    std::string name;
    TransformData* transform_data = nullptr;
    std::vector<int> children;
    std::string id;
    
    ~TreeNodeOptions() {
        MN_SAFE_DELETE(transform_data);
    }
};

struct TreeOptions {
    std::vector<std::shared_ptr<TreeNodeOptions>> nodes;
    std::vector<int> children;
};

class Tree : public BaseContentItem {
    
public:
    
    Tree(std::shared_ptr<TreeOptions> options, std::shared_ptr<Transform> root_transform);
    
    ~Tree();
    
//    std::shared_ptr<Transform> GetNodeTransform();
    
    std::shared_ptr<TreeNode> GetNodeById(const std::string& id);
    
    std::shared_ptr<Transform> GetNodeTransform(const std::string& id);
    
private:
    
    void Build(std::shared_ptr<TreeOptions> options);
    
    std::vector<std::shared_ptr<TreeNode>> nodes_;
    
    std::vector<std::shared_ptr<TreeNode>> all_nodes_;
    
    std::unordered_map<std::string, std::shared_ptr<TreeNode>> cache_map_;
    
    std::shared_ptr<Transform> tree_transform_;

};


}

#endif /* tree_item_hpp */
