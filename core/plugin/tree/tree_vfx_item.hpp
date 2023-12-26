//
//  tree_vfx_item.hpp
//
//  Created by Zongming Liu on 2022/10/13.
//

#ifndef tree_vfx_item_hpp
#define tree_vfx_item_hpp

#include <stdio.h>
#include <vector>
#include "player/vfx_item.h"
#include "math/transform.h"
#include "plugin/tree/tree_item.hpp"
#include "model/common.hpp"

namespace mn {

struct TreeItemOptions {
    std::shared_ptr<TreeOptionsAnimEx> tree;
};

class TreeVFXItem : public VFXItem {
    
public:
    
    TreeVFXItem();
    
    ~TreeVFXItem();
    
    std::string GetType() const override {
        return VFX_ITEM_TYPE_TREE;
    }
    
    void OnConstructed(ItemData* arg) override;
    
    BaseContentItem* InnerCreateContent() override;
    
    void InnerOnUpdate(float dt, std::shared_ptr<VFXItem> thiz) override;

    void OnUpdateByLoader(float dt, std::shared_ptr<VFXItem> thiz) override;
    
public:
    
    std::shared_ptr<TreeItemOptions> options_;
    
};
}

#endif /* tree_vfx_item_hpp */
