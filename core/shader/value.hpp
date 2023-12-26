//
//  value.hpp
//
//  Created by Zongming Liu on 2022/3/21.
//

#ifndef value_hpp
#define value_hpp

#include <stdio.h>

const std::string VALUE_FUNC = R"(

#ifdef SHADER_VERTEX
    #define CURVE_VALUE_TEXTURE uVCurveValueTexture
    #define CURVE_VALUE_ARRAY uVCurveValues
    #define CURVE_VALUE_COUNT VERT_CURVE_VALUE_COUNT
#else
    #define CURVE_VALUE_TEXTURE uFCurveValueTexture
    #define CURVE_VALUE_ARRAY uFCurveValues
    #define CURVE_VALUE_COUNT FRAG_CURVE_VALUE_COUNT
#endif

#if CURVE_VALUE_COUNT > 0
    #if LOOKUP_TEXTURE_CURVE
        uniform sampler2D CURVE_VALUE_TEXTURE;
        const float uCurveCount = 1./ float(CURVE_VALUE_COUNT);
        #define lookup_curve(i) texture2D(CURVE_VALUE_TEXTURE,vec2(float(i) * uCurveCount,0.))
    #else
        uniform vec4 CURVE_VALUE_ARRAY[CURVE_VALUE_COUNT];
        #define lookup_curve(i) CURVE_VALUE_ARRAY[i]
    #endif
#else
#define lookup_curve(i) vec4(0.)
#endif

#ifdef SHADER_VERTEX
    attribute float aSeed;
    varying float vSeed;
    #define NONE_CONST_INDEX 1
#else
    #if LOOKUP_TEXTURE_CURVE
    #define NONE_CONST_INDEX 1
    #endif
#endif

#ifdef NONE_CONST_INDEX
    #ifdef SHADER_VERTEX
        #define MAX_C VERT_MAX_KEY_FRAME_COUNT
    #else
        #define MAX_C FRAG_MAX_KEY_FRAME_COUNT
    #endif
#else
    #define MAX_C  CURVE_VALUE_COUNT
#endif

float evaluateCurveFrames(float time, vec4 keyframe0, vec4 keyframe1){
    float dt = keyframe1.x - keyframe0.x;

    float m0 = keyframe0.w * dt;
    float m1 = keyframe1.z * dt;

    float t = (time - keyframe0.x) / dt;
    float t2 = t * t;
    float t3 = t2 * t;

    return dot(vec4(dot(vec3(2., -3., 1.), vec3(t3, t2, 1.)), dot(vec3(1, -2., 1), vec3(t3, t2, t)), t3 - t2, dot(vec2(-2, 3), vec2(t3, t2))), vec4(keyframe0.y, m0, m1, keyframe1.y));
}

float valueFromCurveFrames(float time, float frameStart, float frameCount){
    int start = int(frameStart);
    int count = int(frameCount - 1.);
    int end = start + count;
    for (int i = 0;i < MAX_C;i++){
        #ifdef NONE_CONST_INDEX
        if (i == count){
            return lookup_curve(i).y;
        }
        vec4 k0 = lookup_curve(i+start);
        vec4 k1 = lookup_curve(i+1+start);
        #else
        if (i < start){
            continue;
        }
        vec4 k0 = lookup_curve(i);
        vec4 k1 = lookup_curve(i+1);
        if (i == end){
            return k0.y;
        }
            #endif
        if (time >= k0.x && time <= k1.x){
            return evaluateCurveFrames(time, k0, k1);
        }
    }
    return lookup_curve(0).y;
}

float evaluteLineSeg(float t, vec2 p0, vec2 p1){
    return p0.y+(p1.y-p0.y)*(t-p0.x)/(p1.x-p0.x);
}

float valueFromLineSegs(float time, float frameStart, float frameCount){
    int start = int(frameStart);
    int count = int(frameCount - 1.);
    int end = start + count;
    for (int i = 0;i < MAX_C;i++){
        #ifdef NONE_CONST_INDEX
        if (i > count){
            return lookup_curve(i).w;
        }
            #else
        if (i<start){
            continue;
        }
        if (i > end){
            return lookup_curve(i-2).w;
        }
            #endif

            #ifdef NONE_CONST_INDEX
        vec4 seg = lookup_curve(i+start);
        #else
        vec4 seg = lookup_curve(i);
        #endif
        vec2 p0 = seg.xy;
        vec2 p1 = seg.zw;
        if (time >= p0.x && time <= p1.x){
            return evaluteLineSeg(time, p0, p1);
        }
            #ifdef NONE_CONST_INDEX
        vec2 p2 = lookup_curve(i+start+1).xy;
        #else
        vec2 p2 = lookup_curve(i+1).xy;
        #endif
        if (time > p1.x && time <= p2.x){
            return evaluteLineSeg(time, p1, p2);
        }
    }
    return lookup_curve(0).y;
}

float getValueFromTime(float time, vec4 value){
    float type = value.x;
    if (type == 0.){
        return value.y;
    }
    else if (type == 1.){
        return mix(value.y, value.z, time / value.w);
    }
    if (type == 2.){
        return valueFromCurveFrames(time, floor(value.y), floor(1./fract(value.y)+0.5)) * value.w + value.z;
    }
    if (type == 3.){
        return valueFromLineSegs(time, value.y, value.z);
    }
    if (type == 4.){
        #ifdef SHADER_VERTEX
        float seed = aSeed;
        #else
        float seed = vSeed;
        #endif
        return mix(value.y, value.z, seed);
    }
    return 0.;
}

)";

#endif /* value_hpp */
