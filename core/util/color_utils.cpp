//
//  color_utils.cpp
//
//  Created by Zongming Liu on 2022/1/26.
//

#include "color_utils.hpp"
#include "util/log_util.hpp"
#include <math.h>
#include <cmath>

namespace mn {

void ColorUtils::ParsePercent() {
    
}

void ColorUtils::InterpolateColor(Vec4& out, const Vec4& a, const Vec4& b, float time, bool origin) {
    float ms = 1 - time;
    if (origin) {
        for (size_t i=0; i<4; i++) {
            out.m[i] = a.m[i] * ms + b.m[i] * time;
        }
    } else {
        for (size_t i=0; i<3; i++) {
            out.m[i] = std::round(sqrt(a.m[i] * a.m[i] * ms + b.m[i] * b.m[i] * time));
        }
        out.m[3] = std::round(a.m[3] * ms + b.m[3] * time);
    }
}

void ColorUtils::GetColorFromGradientStops(const std::vector<ColorStop>& color_stops, Vec4& color_inc, float life_time, bool normalize) {
    size_t stop_len = color_stops.size();
    if (stop_len > 0) {
        bool match = false;
        for (size_t j = 1; j<=stop_len-1; j++) {
            const ColorStop& stop_0 = color_stops[j - 1];
            const ColorStop& stop_1 = color_stops[j];
            
            if (stop_0.stop <= life_time && life_time <= stop_1.stop) {
                ColorUtils::InterpolateColor(color_inc, stop_0.color, stop_1.color, (life_time-stop_0.stop)/(stop_1.stop - stop_0.stop), false);
                match = true;
                break;
            }
        }
        
        if (!match) {
            color_inc = color_stops[stop_len-1].color;
        }
        
        if (normalize) {
            color_inc.Divide(255.0);
        }
    } else {
        
    }
}

void ColorUtils::ImageDataFromGradient(ImageTextureData& image_data, const std::vector<ColorStop>& color_stops) {
    size_t stop_len = color_stops.size();
    int width = image_data.width;

    if (!color_stops.size()) {
        memset(image_data.data, 0, image_data.width * image_data.height * 4);
        return;
    }

    image_data.data[0] = (uint8_t) color_stops[0].color.m[0];
    image_data.data[1] = (uint8_t) color_stops[0].color.m[1];
    image_data.data[2] = (uint8_t) color_stops[0].color.m[2];
    image_data.data[3] = (uint8_t) color_stops[0].color.m[3];
    for (int i = 1; i < width - 1; i++) {
        float index = (float) i/width;
        ColorStop stop_0;
        ColorStop stop_1;
        for (size_t j = 0; j < stop_len - 1; j++) {
            stop_0 = color_stops[j];
            stop_1 = color_stops[j + 1];
            if (stop_0.stop <= index && stop_1.stop > index) {
                break;
            }
        }
        
        Vec4 color;
        ColorUtils::InterpolateColor(color, stop_0.color, stop_1.color, (index - stop_0.stop) / (stop_1.stop - stop_0.stop), false);
        
        // float转化为uint
        uint8_t r = (uint8_t) color.m[0];
        uint8_t g = (uint8_t) color.m[1];
        uint8_t b = (uint8_t) color.m[2];
        uint8_t a = (uint8_t) color.m[3];
        
        int idx = 4 * i;
        image_data.data[idx + 0] = r;
        image_data.data[idx + 1] = g;
        image_data.data[idx + 2] = b;
        image_data.data[idx + 3] = a;
    }

    int idx = 4 * (width - 1);
    image_data.data[idx + 0] = (uint8_t) color_stops[stop_len - 1].color.m[0];
    image_data.data[idx + 1] = (uint8_t) color_stops[stop_len - 1].color.m[1];
    image_data.data[idx + 2] = (uint8_t) color_stops[stop_len - 1].color.m[2];
    image_data.data[idx + 3] = (uint8_t) color_stops[stop_len - 1].color.m[3];
}

Vec4 ColorUtils::ToPluginColor(const Vec4& color) {
    float r = ColorUtils::ScaleTo1(color.m[0]);
    float g = ColorUtils::ScaleTo1(color.m[1]);
    float b = ColorUtils::ScaleTo1(color.m[2]);
    float a = ColorUtils::ScaleTo1(color.m[3]);
    
    Vec4 ret(r, g, b, a);
    return ret;
}

int ColorUtils::ScaleTo255(float val) {
    return (int) std::round(val * 255.0f);
}

float ColorUtils::ScaleTo1(float val) {
    return val/ 255.0f;
}



}
