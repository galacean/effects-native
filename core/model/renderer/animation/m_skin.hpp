//
//  m_skin.hpp
//
//  Created by Zongming Liu on 2022/10/12.
//

#ifndef m_skin_hpp
#define m_skin_hpp

#include <stdio.h>
#include <vector>
#include "model/common.hpp"
#include "plugin/tree/tree_vfx_item.hpp"

namespace mn {

class MSkin {
    
public:
    
    MSkin(MarsRenderer* renderer);
    
    ~MSkin();
    
    void Create(std::shared_ptr<MarsSkinOptions> options, std::shared_ptr<TreeVFXItem> parent_item);
    
    void UpdateSkinMatrices();
    
    void UpdateParentItem(std::shared_ptr<TreeVFXItem> parent_item);
    
    size_t JointCount() {
        return joint_list_.size();
    }
    
    const std::vector<Mat4>& AnimationMatrices() {
        return animation_matrices_;
    }
    
    bool IsTextureMode();

private:
    
    int skeleton_;
    
    std::vector<int> joint_list_;
    
    std::vector<Mat4> inverse_bind_matrices_;
    
    std::vector<Mat4> animation_matrices_;
    
    std::weak_ptr<TreeVFXItem> parent_item_;
    
    MarsRenderer* renderer_;
    
    bool force_skin_texture_ = false;
    
};

}


#endif /* m_skin_hpp */
