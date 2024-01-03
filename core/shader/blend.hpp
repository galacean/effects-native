//
//  blend.hpp
//
//  Created by Zongming Liu on 2022/3/16.
//

#ifndef blend_hpp
#define blend_hpp

#include <stdio.h>

const std::string BLEND_FUNC = R"(

vec4 blendColor(vec4 color,vec4 vc,float mode){
    vec4 ret = color * vc;
    #ifdef PRE_MULTIPLY_ALPHA
    float alpha = vc.a;
    #else
    float alpha = ret.a;
    #endif
    if (mode == 1.){
        ret.rgb *= alpha;
    } else if (mode == 2.){
        ret.rgb *= alpha;
        ret.a = dot(ret.rgb, vec3(0.33333333));
    } else if(mode == 3.){
        alpha = color.r * alpha;
        ret = vec4(vc.rgb * alpha, alpha) ;
    } else {
        ret = vec4(ret.rgb, ret.a);
    }
    return ret;
}

)";

#endif /* blend_hpp */
