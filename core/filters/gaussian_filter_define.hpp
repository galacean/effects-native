//
//  gaussian_filter_define.hpp
//  MarsNative
//
//  Created by Zongming Liu on 2022/7/27.
//  Copyright © 2022 Alipay. All rights reserved.
//

#ifndef gaussian_filter_define_hpp
#define gaussian_filter_define_hpp

#include <stdio.h>
#include "filters/filter_define.hpp"

namespace mn {

struct GaussainParams {
    int radius;
    int step;
    std::string fragment;
    int down_sample;
};

class GaussianFilterDefine : public FilterDefine {
  
public:
    
    static void GenerateGaussianFragment(int radius, GaussainParams& gaussian_params);
    
    GaussianFilterDefine(Composition* composition, GaussianFilterData* filter_data);
    
    ~GaussianFilterDefine();
    
    std::vector<std::shared_ptr<RenderPass>> GeneratePreRenderPasses(std::vector<std::shared_ptr<MarsMesh>> meshes) override;
    
    float Step() {
        return step_;
    }
    
    std::shared_ptr<MarsTexture> GetGaussianVTexture() {
        return gaussian_texture_v_;
    }
    
    std::shared_ptr<MarsTexture> GetGaussianHTexture() {
        return gaussian_textuer_h_;
    }
        
private:
    
    static std::vector<float> GetGaussianParams(int radius);
    
    static float GaussianNumber(int x, float sig);
    
    std::vector<std::shared_ptr<MarsMesh>> GeneratePassMeshes(std::vector<std::shared_ptr<MarsMesh>> meshes);
    
    std::shared_ptr<MarsTexture> gaussian_texture_v_;
    
    std::shared_ptr<MarsTexture> gaussian_textuer_h_;
    
    std::string gaussian_fragment_ = "";
    
    int viewport_[4] = { 0, 0, 0, 0 };
    
    float radiu_;
    
    float step_;
    
};

}

#endif /* gaussian_filter_define_hpp */
