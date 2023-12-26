//
//  interpolation_sampler.hpp
//
//  Created by Zongming Liu on 2022/10/9.
//

#ifndef interpolation_sampler_hpp
#define interpolation_sampler_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include "model/common.hpp"

namespace mn {

class InterpolationSampler {

public:

    static std::shared_ptr<InterpolationSampler> CreateAnimationSampler(MAnimMethodType method, MAnimTargetType target, uint8_t* time_data, size_t time_byte_length,
                                                                        uint8_t* data, size_t data_byte_lenght, size_t component);
    
    static void SlerpFlat(float* dst, size_t dst_offset, std::vector<float> source0, size_t src_offset_0, std::vector<float> source1, size_t src_offset_1, float t);
    
    InterpolationSampler(uint8_t* time_data, uint8_t* data, size_t count, size_t component);
    
    ~InterpolationSampler();
    
    float* Evaluate(float t);
    
protected:
    
    void CopySampleValue();
    
    virtual void IntervalChanged() = 0;
    
    virtual float* InterPolate(size_t seg_idx, float time, float t0, float t1) = 0;
   
    std::vector<float> time_array_;
    std::vector<float> data_array_;
    size_t count_;
    size_t component_count_;
};

class LinearSampler : public InterpolationSampler {
    
public:
    
    LinearSampler(uint8_t* time_data, uint8_t* data, size_t count, size_t component);
    
    ~LinearSampler();
    
    void IntervalChanged() override;
    
    float* InterPolate(size_t seg_idx, float time, float t0, float t1) override;
    
};

class QuaternionLinearSampler : public InterpolationSampler {
    
public:
    
    QuaternionLinearSampler(uint8_t* time_data, uint8_t* data, size_t count, size_t component);
    
    ~QuaternionLinearSampler();
    
    void IntervalChanged() override;
    
    float* InterPolate(size_t seg_idx, float time, float t0, float t1) override;
        
};

class DiscreteSampler : public InterpolationSampler {
    
public:
    
    DiscreteSampler(uint8_t* time_data, uint8_t* data, size_t count, size_t component);
    
    ~DiscreteSampler();
    
    void IntervalChanged() override;
    
    float* InterPolate(size_t seg_idx, float time, float t0, float t1) override;
    
};

}

#endif /* interpolation_sampler_hpp */
