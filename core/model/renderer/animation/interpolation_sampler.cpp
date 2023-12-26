//
//  interpolation_sampler.cpp
//
//  Created by Zongming Liu on 2022/10/9.
//

#include "interpolation_sampler.hpp"
#include "util/log_util.hpp"

namespace mn {

std::shared_ptr<InterpolationSampler> InterpolationSampler::CreateAnimationSampler(MAnimMethodType method, MAnimTargetType target, uint8_t* time_data, size_t time_byte_length,
                                                                                   uint8_t* data, size_t data_byte_lenght, size_t component) {
    size_t time_count = time_byte_length / sizeof(float);
    size_t data_count = data_byte_lenght / (sizeof(float) * component);
    
    if (time_count != data_count) {
        MLOGE("Interpolation Sampler Generate Failed");
        return nullptr;
    }
    
    if (method == MAnimMethodType::LINEAR) {
        if (target == MAnimTargetType::ROTATION) {
            return std::make_shared<QuaternionLinearSampler>(time_data, data, time_count, component);
        } else {
            return std::make_shared<LinearSampler>(time_data, data, time_count, component);
        }
    } else if (method == MAnimMethodType::STEP) {
        return std::make_shared<DiscreteSampler>(time_data, data, time_count, component);
    } else if (method == MAnimMethodType::CUBICSPLINE) {
        assert(false);
    }
    
    return nullptr;
}


void InterpolationSampler::SlerpFlat(float* dst, size_t dst_offset, std::vector<float> source0, size_t src_offset_0, std::vector<float> source1, size_t src_offset_1, float t) {
    
    float x0 = source0[src_offset_0 + 0];
    float y0 = source0[src_offset_0 + 1];
    float z0 = source0[src_offset_0 + 2];
    float w0 = source0[src_offset_0 + 3];

    float x1 = source1[src_offset_1 + 0];
    float y1 = source1[src_offset_1 + 1];
    float z1 = source1[src_offset_1 + 2];
    float w1 = source1[src_offset_1 + 3];
    
    if (t == 0) {

      dst[dst_offset + 0] = x0;
      dst[dst_offset + 1] = y0;
      dst[dst_offset + 2] = z0;
      dst[dst_offset + 3] = w0;

      return;

    }
    
    if (t == 1) {

          dst[dst_offset + 0] = x1;
          dst[dst_offset + 1] = y1;
          dst[dst_offset + 2] = z1;
          dst[dst_offset + 3] = w1;

          return;

    }
    
    if (w0 != w1 || x0 != x1 || y0 != y1 || z0 != z1) {

        float s = 1 - t;
        float cos = x0 * x1 + y0 * y1 + z0 * z1 + w0 * w1;
        float dir = (cos >= 0 ? 1 : - 1);
        float sqrSin = 1 - cos * cos;

          // Skip the Slerp for tiny steps to avoid numeric problems:
          if (sqrSin > std::numeric_limits<float>::epsilon()) {

              float sin = std::sqrt(sqrSin);
            float len = std::atan2(sin, cos * dir);

            s = std::sin(s * len) / sin;
            t = std::sin(t * len) / sin;

          }

          float tDir = t * dir;

          x0 = x0 * s + x1 * tDir;
          y0 = y0 * s + y1 * tDir;
          z0 = z0 * s + z1 * tDir;
          w0 = w0 * s + w1 * tDir;

          // Normalize in case we just did a lerp:
          if (s == 1 - t) {

            float f = 1 / std::sqrt(x0 * x0 + y0 * y0 + z0 * z0 + w0 * w0);

            x0 *= f;
            y0 *= f;
            z0 *= f;
            w0 *= f;

          }

        }

        dst[dst_offset] = x0;
        dst[dst_offset + 1] = y0;
        dst[dst_offset + 2] = z0;
        dst[dst_offset + 3] = w0;
}

InterpolationSampler::InterpolationSampler(uint8_t* time_data, uint8_t* data, size_t count, size_t component) {
    float* time_float_data = (float *) time_data;
    float* data_float_data = (float *) data;
    component_count_ = component;
    count_ = count;
    
    for (size_t i=0; i<count; i++) {
        time_array_.push_back(time_float_data[i]);
        for (size_t j=0; j<component_count_; j++) {
            data_array_.push_back(data_float_data[i * component_count_+ j]);
        }
    }
}

InterpolationSampler::~InterpolationSampler() {
    
}

float* InterpolationSampler::Evaluate(float time) {
    size_t index = 0;
    if (time < time_array_[0]) {
        size_t byte_length = component_count_ * sizeof(float);
        uint8_t* result = (uint8_t *) malloc(byte_length);
        memset(result, 0, byte_length);
        float* float_result = (float *)result;
        
        for (size_t i=0; i<component_count_; i++) {
            float_result[i] = data_array_[0 + i];
        }
        
        return float_result;
    }
    
    if (time > time_array_[count_ - 1]) {
        size_t byte_length = component_count_ * sizeof(float);
        uint8_t* result = (uint8_t *) malloc(byte_length);
        memset(result, 0, byte_length);
        float* float_result = (float *)result;

        for (size_t i=0; i<component_count_; i++) {
            float_result[i] = data_array_[(count_ - 1) + i];
        }
        
        return float_result;
    }
    
    while(index < count_ - 1) {
        index++;
        if (time >= time_array_[index - 1] && time < time_array_[index]) {
            break;
        }
    }
    
    
    return this->InterPolate(index, time, time_array_[index - 1], time_array_[index]);
}

LinearSampler::LinearSampler(uint8_t* time_data, uint8_t* data, size_t count, size_t component) : InterpolationSampler(time_data, data, count, component) {
    
}

LinearSampler::~LinearSampler() {
    
}

void LinearSampler::IntervalChanged() {
    
}

float* LinearSampler::InterPolate(size_t i1, float t, float t0, float t1) {
    size_t byte_length = component_count_ * sizeof(float);
    uint8_t* result = (uint8_t *) malloc(byte_length);
    memset(result, 0, byte_length);
    
    float* float_result = (float*) result;
    float weight1 = (t - t0) / (t1 - t0);
    float weight0 = 1 - weight1;
    
    size_t offset1 = i1 * component_count_;
    size_t offset0 = offset1 - component_count_;
    for (size_t i=0; i<component_count_; i++) {
        float_result[i] = data_array_[offset0 + i] * weight0 + data_array_[offset1 + i] * weight1;
    }
    
    DEBUG_DATA("LinearSampler InterPolate", result, byte_length, sizeof(float));
    return float_result;
}

QuaternionLinearSampler::QuaternionLinearSampler(uint8_t* time_data, uint8_t* data, size_t count, size_t component) : InterpolationSampler(time_data, data, count, component) {
    
    
}

QuaternionLinearSampler::~QuaternionLinearSampler() {
    
}

void QuaternionLinearSampler::IntervalChanged() {
    
}

// todo:
float* QuaternionLinearSampler::InterPolate(size_t seg_idx, float time, float t0, float t1) {
    size_t byte_len = component_count_ * sizeof(float);
    uint8_t* result = (uint8_t *) malloc(byte_len);
    memset(result, 0, byte_len);
    
    float alpha = (time - t0) / (t1 - t0);
    size_t offset = seg_idx * component_count_;
    
    float* float_result = (float *)result;
    InterpolationSampler::SlerpFlat(float_result, 0, data_array_, offset - component_count_, data_array_, offset, alpha);
    
    DEBUG_DATA("QuaternionSampler InterPolate", result, byte_len, sizeof(float));
    return float_result;
}

DiscreteSampler::DiscreteSampler(uint8_t* time_data, uint8_t* data, size_t count, size_t component) : InterpolationSampler(time_data, data, count, component) {
    
}

DiscreteSampler::~DiscreteSampler() {
    
}

void DiscreteSampler::IntervalChanged() {
    
}

// todo:
float* DiscreteSampler::InterPolate(size_t seg_idx, float time, float t0, float t1) {
    size_t byte_length = component_count_ * sizeof(float);
    uint8_t* result = (uint8_t *) malloc(byte_length);
    memset(result, 0, byte_length);
    
    size_t offset = (seg_idx - 1) * component_count_;
    for (size_t i=0; i < component_count_; i++) {
        result[i] = data_array_[offset + i];
    }
    
    DEBUG_DATA("DiscreateSampler InterPolate", result, byte_length, sizeof(float));
    
    return (float *) result;
}

}

