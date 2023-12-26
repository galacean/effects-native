//
//  filter_utils.hpp
//  MarsNative
//
//  Created by Zongming Liu on 2022/7/19.
//  Copyright © 2022 Alipay. All rights reserved.
//

#ifndef filter_utils_hpp
#define filter_utils_hpp

#include <stdio.h>
#include <string>
#include "ri/render/mars_mesh.hpp"
#include "player/composition.h"

namespace mn {

class FilterUtils {
    
public:
    
    static std::shared_ptr<FilterDefine> GenerateFilter(Composition* composition, ItemFilterData* filter_data);

    
    static std::shared_ptr<MarsMesh> CloneSpriteMesh(std::shared_ptr<MarsMesh> clone_mesh, Composition* composition, FilterDefine* filter_define);
        
private:
    
    static std::shared_ptr<MarsMesh> CloneMeshWithShader(std::shared_ptr<MarsMesh> clone_mesh, Composition* composition, const Shader& shader);
    
};
}

#endif /* filter_utils_hpp */
