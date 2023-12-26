//
//  bloom_filter_define.hpp
//  MarsNative
//
//  Created by Zongming Liu on 2022/8/2.
//  Copyright © 2022 Alipay. All rights reserved.
//

#ifndef bloom_filter_define_hpp
#define bloom_filter_define_hpp

#include <stdio.h>
#include "filters/filter_define.hpp"

namespace mn {

class BloomFilterDefine : public FilterDefine {
    
public:
    
    BloomFilterDefine(Composition* composition, BloomFilterData* filter_data);
    
    ~BloomFilterDefine();
    
    void OnItemRemove() override;
    
    std::vector<std::shared_ptr<RenderPass>> GeneratePreRenderPasses(std::vector<std::shared_ptr<MarsMesh>> meshes) override;
    
    void GetVariableUniformValue(const std::string& key, float life, std::shared_ptr<UniformValue> value) override;
    
    // todo:优化;
    std::shared_ptr<MarsTexture> blur_target_;
    
    std::shared_ptr<MarsTexture> blur_inner_media_;
    
private:
    
    std::vector<std::shared_ptr<MarsMesh>> GenerateThresholdMesh(std::vector<std::shared_ptr<MarsMesh>> meshes);
    
    std::vector<std::shared_ptr<MarsMesh>> GenerateGaussianMesh(std::vector<std::shared_ptr<MarsMesh>> meshes);
    
    ValueGetter* bloom_add_on_ = nullptr;
    
    ValueGetter* color_add_on_ = nullptr;
    
    Vec3 color_weights_;
    
    int viewport_[4] = { 0, 0, 0, 0 };
    
    std::string gaussian_fragment_ = "";
    
};

}

#endif /* bloom_filter_define_hpp */
