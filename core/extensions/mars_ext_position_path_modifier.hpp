//
//  mars_ext_position_path_modifier.hpp
//  MarsNative
//
//  Created by changxing on 2023/9/12.
//  Copyright © 2023 Alipay. All rights reserved.
//

#ifndef mars_ext_position_path_modifier_hpp
#define mars_ext_position_path_modifier_hpp

#include <string>
#include <vector>

namespace mn {

class SceneDataDecoder;
class CompositionData;

class MarsExtPositionPathModifier {
public:
    MarsExtPositionPathModifier();
    
    ~MarsExtPositionPathModifier();
    
    bool SetSceneData(SceneDataDecoder* scene_data);
    
    bool GetPositionPathByName(const std::string& name, float** data, int* length, float** data2, int* length2);
    
    void ModifyPositionPath(const std::string& name, float* value, int length, float* value2, int length2);
    
    inline float GetPixelRatio() const {
        return pixel_ratio_;
    }
    
    inline float GetAspect() const {
        return aspect_;
    }
    
    inline float GetAnimWidth() const {
        return anim_width_;
    }
    
    inline float GetAnimHeight() const {
        return anim_height_;
    }
    
private:
    CompositionData* composition_ = nullptr;
    float pixel_ratio_ = 1;
    float aspect_ = 1;
    float anim_width_ = 1;
    float anim_height_ = 1;
    
    std::vector<float*> data_to_free_;
};

}

#endif /* mars_ext_position_path_modifier_hpp */
