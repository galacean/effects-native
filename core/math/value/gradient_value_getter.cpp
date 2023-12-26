//
//  gradient_value_getter.cpp
//
//  Created by Zongming Liu on 2022/2/21.
//

#include "gradient_value_getter.hpp"
#include "util/log_util.hpp"

namespace mn {

GradientValueGetter::GradientValueGetter() {
    type_ = ValueGetterType::GRADIENT;
    MLOGD("GradientValueGetter Construct");
}

GradientValueGetter::~GradientValueGetter() {
    MLOGD("GradientValueGetter Destruct");
}

void GradientValueGetter::AddColorStop(const ColorStop& color_stop) {
    stops_.push_back(color_stop);
}
  
float GradientValueGetter::GetValue(float t) {
    return 1.0;
}

void GradientValueGetter::GetValues(float t, float* ret, size_t length) {
    size_t len = stops_.size();
    for (size_t i = 0; i < len - 1; i++) {
        ColorStop a = stops_[i];
        ColorStop b = stops_[i + 1];
        if (a.stop <= t && b.stop >= t) {
            float dt = (t - a.stop) / (b.stop - a.stop);
            Vec4 ret_color;
            ColorUtils::InterpolateColor(ret_color, a.color, b.color, dt, true);
            ret_color.Copy(ret);
            return;
        }
    }
    
    ColorStop last = stops_[len - 1];
    last.color.Copy(ret);
    return ;
}

float GradientValueGetter::GetIntegrateValue(float t0, float t1, float time_scale) {
    return 1.0;
}

float GradientValueGetter::GetIntegrateByTime(float t0, float t1) {
    return 1.0;
}

}
