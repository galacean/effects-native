//
//  delay_filter_define.hpp
//  MarsNative
//
//  Created by Zongming Liu on 2022/8/2.
//  Copyright © 2022 Alipay. All rights reserved.
//

#ifndef delay_filter_define_hpp
#define delay_filter_define_hpp

#include <stdio.h>
#include <unordered_map>
#include <functional>
#include "filters/filter_define.hpp"

namespace mn {

class DelayFilterDefine : public FilterDefine {
    
public:
    
    DelayFilterDefine(Composition* composition, DelayFilterData* filter_data);
    
    ~DelayFilterDefine();
    
    std::vector<std::shared_ptr<RenderPass>> GeneratePreRenderPasses(std::vector<std::shared_ptr<MarsMesh>> meshes) override;
    
    void GetVariableUniformValue(const std::string& key, float life, std::shared_ptr<UniformValue> value) override;

private:
    
    std::shared_ptr<MarsTexture> last_source_texture_;
    
    float filter_params_[4] = { 0.0, 0.96, 0.0, 0.0 };
    
};

}

#endif /* delay_filter_define_hpp */
