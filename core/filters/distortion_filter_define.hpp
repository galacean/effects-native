//
//  distortion_filter_define.hpp
//  MarsNative
//
//  Created by Zongming Liu on 2022/8/2.
//  Copyright © 2022 Alipay. All rights reserved.
//

#ifndef distortion_filter_define_hpp
#define distortion_filter_define_hpp

#include <stdio.h>
#include "filters/filter_define.hpp"

namespace mn {

class DistortionFilterDefine : public FilterDefine {
    
public:
    
    DistortionFilterDefine(Composition* composition, DistortionFilterData* filter_data);
    
    ~DistortionFilterDefine();
    
    std::vector<std::shared_ptr<RenderPass>> GeneratePreRenderPasses(std::vector<std::shared_ptr<MarsMesh>> meshes) override;
    
    void GetVariableUniformValue(const std::string& key, float life, std::shared_ptr<UniformValue> value) override;
    
private:
    float center_[2] = { -0.5, 0.5 };
    float direction_[2] { 1.0, 1.0 };
    // 周期
    ValueGetter* u_period_value_ = nullptr;
    // 强度
    ValueGetter* u_strength_value_ = nullptr;
    // 波化
    ValueGetter* u_movement_value_ = nullptr;
};

}

#endif /* distortion_filter_define_hpp */
