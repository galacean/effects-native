//
//  line_segments_value_getter.cpp
//
//  Created by Zongming Liu on 2022/1/13.
//

#include <cmath>
#include <algorithm>

#include "line_segments_value_getter.hpp"
#include "util/log_util.hpp"
#include "math/value/value_getter_utils.hpp"

namespace mn {

static void SortLineSegmentDatas(line_arr_2d arr, int len) {
    if (len < 2) {
        return;
    }
    // 原始数据顺序基本正确，只有小范围改动，简单排序
    for (int i = len - 2; i >= 0; i--) {
        int j = i;
        while ((j < len - 1) && (arr[j][0] > arr[j + 1][0])) {
            float temp = arr[j][0];
            arr[j][0] = arr[j + 1][0];
            arr[j + 1][0] = temp;
            temp = arr[j][1];
            arr[j][1] = arr[j + 1][1];
            arr[j + 1][1] = temp;
            ++j;
        }
    }
}

LineSegmentsValueGetter::LineSegmentsValueGetter(line_arr_2d value, size_t len) {
    key_values_ = value;
    length_ = len;
    type_ = ValueGetterType::LINEAR_SEGMENT_TYPE;
    
    SortLineSegmentDatas(key_values_, (int) len);
}

LineSegmentsValueGetter::~LineSegmentsValueGetter() {
    if (key_values_) {
        delete[] key_values_;
    }
}

float LineSegmentsValueGetter::GetValue(float time) {
    if (time < key_values_[0][0]) {
        return key_values_[0][1];
    }
    
    size_t end = length_ - 1;
    for (size_t i=0; i<length_-1; i++) {
        auto key0 = key_values_[i];
        auto key1 = key_values_[i+1];
        
        float time0 = key0[0];
        float time1 = key1[0];
        
        if (time >= time0 && time <= time1) {
            float p = (time - time0) / (time1 - time0);
            float value0  = key0[1];
            return value0 + p * (key1[1] - value0);
        }
    }
    return key_values_[end][1];
}

void LineSegmentsValueGetter::GetValues(float t, float* ret, size_t length) {
    MLOGE("LineSegmentsValueGetter GetValues not supported");
}

float LineSegmentsValueGetter::GetIntegrateValue(float t0, float t1, float time_scale) {
    return this->Integrate(t1, false, time_scale) - this->Integrate(t0, false, time_scale);
}

float LineSegmentsValueGetter::GetIntegrateByTime(float t0, float t1) {
    return this->Integrate(t1, true, 1) - this->Integrate(t0, true, 1);
}

void LineSegmentsValueGetter::ToUniform(KeyFrameMeta& meta, float* ret) {
    float index = meta.index;
    float uniform_count = std::ceil((length_ / 2.0f));
    
    meta.line_seg_count += uniform_count;
    meta.index += uniform_count;
    meta.max = std::max(meta.max, uniform_count);
    
    for (size_t i=0; i<uniform_count; i++) {
        Vec4 data;
        size_t key0_index = 2 * i;
        size_t key1_index = 2 * i + 1;
    
        data.m[0] = key_values_[key0_index][0];
        data.m[1] = key_values_[key0_index][1];
        
        if (key1_index < length_) {
            data.m[2] = key_values_[key1_index][0];
            data.m[3] = key_values_[key1_index][1];
        } else {
            data.m[2] = key_values_[key0_index][0];
            data.m[3] = key_values_[key0_index][1];
        }
        meta.frame_datas.push_back(data);
    }

    ret[0] = 3.0f;
    ret[1] = index;
    ret[2] = uniform_count;
    ret[3] = 0.0f;
}

float LineSegmentsValueGetter::Integrate(float time, bool by_time, float ts) {
    if (time <= key_values_[0][0]) {
        return 0.0f;
    }
    
    float ret = 0;
    for (size_t i=0; i<length_-1; i++) {
        auto k1 = key_values_[i];
        auto k2 = key_values_[i + 1];
        
        float t0 = k1[0] * ts;
        float t1 = k2[0] * ts;
       
        if (time > t0 && time <= t1) {
            if (by_time) {
                return ret + ValueGetterUtils::LineSegIntegrateByTime(time, t0, t1, k1[ 1 ], k2[ 1 ]);
            } else {
                return ret + ValueGetterUtils::LineSegIntegrate(time, t0, t1, k1[ 1 ], k2[ 1 ]);
            }
        } else {
            if (by_time) {
                ret += ValueGetterUtils::LineSegIntegrateByTime(t1, t0, t1, k1[1], k2[1]);
            } else {
                ret += ValueGetterUtils::LineSegIntegrate(t1, t0, t1, k1[ 1 ], k2[ 1 ]);
            }
        }
    }
    
    return ret;
}

}
