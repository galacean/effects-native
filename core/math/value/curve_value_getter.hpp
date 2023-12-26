//
//  curve_value_getter.hpp
//
//  Created by Zongming Liu on 2021/12/20.
//

#ifndef curve_value_getter_hpp
#define curve_value_getter_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include "math/value/value_getter.hpp"

#define CURVE_ROW 4

using curve_array_2d = float(*)[CURVE_ROW];

namespace mn {

class CurveValueGetter : public ValueGetter {
    
public:
    
    CurveValueGetter(curve_array_2d values, size_t length);
    
    ~CurveValueGetter();
    
    float GetValue(float t) override;

    void GetValues(float t, float* ret, size_t length) override;

    float GetIntegrateValue(float t0, float t1, float time_scale) override;
    
    float GetIntegrateByTime(float t0, float t1) override;
    
    void ToUniform(KeyFrameMeta& meta, float* ret) override;
    
    static float* GetAllData(KeyFrameMeta& meta);
    
    void ScaleXCoord(float scale) override {
        for (int i = 0; i < length_; i++) {
            key_frames_[i][0] = scale * key_frames_[i][0];
        }
    }
        
private:
    
//    std::vector<float*> key_frames_;
    
    float Integrate(float time, bool by_time);
    
    curve_array_2d key_frames_;
    
    size_t length_;
    
    float min_;
    
    float dist_;
    
};

}
#endif /* curve_value_getter_hpp */
