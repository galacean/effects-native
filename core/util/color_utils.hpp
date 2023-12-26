//
//  color_utils.hpp
//
//  Created by Zongming Liu on 2022/1/26.
//

#ifndef color_utils_hpp
#define color_utils_hpp

#include <stdio.h>
#include <vector>
#include "math/vec4.hpp"

namespace mn {

struct ColorStop {
    ColorStop() {
        stop = 0.0;
    }
    
    ColorStop(float s, Vec4 c) {
        stop = s;
        color = c;
    }
    
    void SetColor(float r, float g, float b, float a) {
        color.Set(r, g, b, a);
    }
    
    float stop;   // 0.0~1.0;
    Vec4 color;
};

struct ImageTextureData {
    
    ImageTextureData(int w, int h) {
        width = w;
        height = h;
        data = (uint8_t *) malloc(w * h * sizeof(float));
    }
    
    ~ImageTextureData() {
        if (data) {
            free(data);
        }
    }
    
    int width;
    
    int height;
    
    uint8_t *data;
    
};

class ColorUtils {

public:
    
    static void ParsePercent();
    
    static void InterpolateColor(Vec4& out, const Vec4& a, const Vec4& b, float time, bool origin);
        
    static void GetColorFromGradientStops(const std::vector<ColorStop>& color_stops, Vec4& color_inc, float life_time, bool normalize);
    
    static void ImageDataFromGradient(ImageTextureData& image_data, const std::vector<ColorStop>& color_stops);
    
    static Vec4 ToPluginColor(const Vec4& color);
    
    static int ScaleTo255(float val);
    
    static float ScaleTo1(float val);
    
};

}

#endif /* color_utils_hpp */
