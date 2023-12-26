//
//  curve_value_getter.cpp
//
//  Created by Zongming Liu on 2021/12/20.
//

#include <algorithm>
#include <limits>
#include "curve_value_getter.hpp"
#include "util/log_util.hpp"
#include "math/value/value_getter_utils.hpp"

namespace mn {

CurveValueGetter::CurveValueGetter(curve_array_2d values, size_t length) {
    length_ = length;
    key_frames_ = values;
    float min = std::numeric_limits<float>::infinity();
    float max = -std::numeric_limits<float>::infinity();
    for (size_t i=0; i<length; i++) {
        auto frame = key_frames_[i];
        min = std::min(min, frame[1]);
        max = std::max(max, frame[1]);
    }
    
    float dist = max - min;
    if (dist != 0.0) {
        for (size_t i=0; i<length; i++) {
            auto frame = key_frames_[i];
            frame[1] = (frame[1] - min) / dist;
        }
    }
    
    min_ = min;
    dist_ = dist;
    type_ = ValueGetterType::CURVE_TYPE;
    
    
}

CurveValueGetter::~CurveValueGetter() {
    if (key_frames_) {
        delete [] key_frames_;
    }
}

float CurveValueGetter::GetValue(float time) {
    if (time <= key_frames_[0][CURVE_PRO_TIME]) {
        return key_frames_[0][CURVE_PRO_VALUE] * dist_ + min_;
    }
    
    size_t end = length_ - 1;
    for (size_t i=0; i<end; i++) {
        auto key0 = key_frames_[i];
        auto key1 = key_frames_[i+1];
        
        if (time > key0[CURVE_PRO_TIME] && time <= key1[CURVE_PRO_TIME]) {
            return ValueGetterUtils::CurveValueEvaluate(time, key0, key1) * dist_ + min_;
        }
    }

    return key_frames_[end][CURVE_PRO_VALUE] * dist_ + min_;
}

void CurveValueGetter::GetValues(float t, float* ret, size_t length) {
    MLOGE("CurveValueGetter GetValues not supported");
}

float CurveValueGetter::GetIntegrateValue(float t0, float t1, float time_scale) {
    float d = (this->Integrate(t1 / time_scale, false) - this->Integrate(t0 / time_scale, false)) * time_scale;
    return this->min_ + d * this->dist_;
}

float CurveValueGetter::GetIntegrateByTime(float t0, float t1) {
    float d = this->Integrate(t1, true) - this->Integrate(t0, true);
    return this->min_ + d * this->dist_;
}

void CurveValueGetter::ToUniform(KeyFrameMeta& meta, float* ret) {
    int index = meta.index;
    float len = (float) length_;
    meta.index += len;
    meta.curve_count += len;
    meta.max = std::max(meta.max, len);
    
    for (size_t i=0; i<length_; i++) {
        Vec4 data;
        data.m[0] = key_frames_[i][0];
        data.m[1] = key_frames_[i][1];
        data.m[2] = key_frames_[i][2];
        data.m[3] = key_frames_[i][3];
        
        meta.frame_datas.push_back(data);
    }
    
    ret[0] = 2.0;
    ret[1] = index + 1.0 / length_;
    ret[2] = min_;
    ret[3] = dist_;
}

float CurveValueGetter::Integrate(float time, bool by_time) {
    if (time <= key_frames_[0][CURVE_PRO_TIME]) {
        return 0.0;
    }
    
    float ret = 0;
    
    for (size_t i=0; i<length_-1; i++) {
        auto k1 = key_frames_[i];
        auto k2 = key_frames_[i+1];
        
        float t1 = k1[CURVE_PRO_TIME];
        float t2 = k2[CURVE_PRO_TIME];
        
        if (time > t1 && time <= t2) {
            if (by_time) {
                return ret + ValueGetterUtils::CurveValueIntegrateByTime(time, k1, k2);
            } else {
                return ret + ValueGetterUtils::CurveValueIntegrate(time, k1, k2);
            }
        } else {
            if (by_time) {
                ret += ValueGetterUtils::CurveValueIntegrateByTime(t2, k1, k2);
            } else {
                ret += ValueGetterUtils::CurveValueIntegrate(t2, k1, k2);
            }
        }
    }
    
    return ret;
}

float* CurveValueGetter::GetAllData(KeyFrameMeta& meta) {
    float* ret = new float[meta.index * 4];
    for (int i = 0; i < meta.frame_datas.size(); i++) {
        ret[i * 4] = meta.frame_datas[i].m[0];
        ret[i * 4 + 1] = meta.frame_datas[i].m[1];
        ret[i * 4 + 2] = meta.frame_datas[i].m[2];
        ret[i * 4 + 3] = meta.frame_datas[i].m[3];
    }
    return ret;
}

}

