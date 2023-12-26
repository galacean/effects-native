//
//  sprite_shader.hpp
//
//  Created by Zongming Liu on 2022/2/7.
//

#ifndef sprite_shader_hpp
#define sprite_shader_hpp

#include <stdio.h>
#include <string>
#include "shader_defines.h"
#include "blend.hpp"

// todo: android上可以用 highp float
const std::string SPRITE_VERTEX_SOURCE = SHADER_PRECISION +
R"(
#define SHADER_VERTEX 1
#define SPRITE_SHADER 1

attribute vec4 aPoint;//x y
attribute vec2 aIndex;//tex

uniform vec4 uPos[MAX_ITEM_COUNT];//xyz
uniform vec2 uSize[MAX_ITEM_COUNT];//width height
uniform vec4 uQuat[MAX_ITEM_COUNT];
uniform vec4 uColor[MAX_ITEM_COUNT];
uniform vec4 uTexParams[MAX_ITEM_COUNT];//transparentOcclusion blending renderMode life
uniform vec4 uTexOffset[MAX_ITEM_COUNT];// x y sx sy

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uViewProjection;

varying vec4 vColor;
varying vec4 vTexCoord;
varying highp vec2 vParams; // texIndex mulAplha

const float d2r = 3.141592653589793 / 180.;

vec4 filterMain(float t, vec4 position);
#pragma FILTER_VERT

vec3 rotateByQuat(vec3 a, vec4 quat){
    vec3 qvec = quat.xyz;
    vec3 uv = cross(qvec, a);
    vec3 uuv = cross(qvec, uv) * 2.;
    return a +(uv * 2. * quat.w + uuv);
}

void main(){
    int index = int(aIndex.x);
    vec4 texParams = uTexParams[index];
    float life = texParams.w;
    if (life < 0. || life > 1.){
        gl_Position = vec4(3., 3., 3., 1.);
    } else {
        vec4 _pos = uPos[index];
        vec3 point = rotateByQuat(vec3(aPoint.xy * uSize[index], 0.),uQuat[index]);
        vec4 pos =  vec4(_pos.xyz, 1.0);

        float renderMode = texParams.z;
        if (renderMode == 0.){
            pos = uModel * pos;
            pos.xyz += uView[0].xyz * point.x + uView[1].xyz * point.y;
        } else if (renderMode == 1.){
            pos.xyz += point;
            pos = uModel * pos;
        } else if (renderMode == 2.){
            pos = uModel * pos;
            pos.xy += point.xy;
        } else if (renderMode == 3.){
            pos = uModel * pos;
            pos.xyz += uView[0].xyz * point.x + uView[2].xyz * point.y;
        }
        gl_Position = uViewProjection * pos;

        #ifdef ADJUST_LAYER
            vec4 filter_Position = filterMain(life,pos);
        #endif

        gl_PointSize = 6.0;

        #ifdef ADJUST_LAYER
            vTexCoord = vec4(filter_Position.xy/filter_Position.w + 1.,gl_Position.xy/gl_Position.w + 1.) / 2.;
        #else
            vec4 texOffset = uTexOffset[index];
            vTexCoord = vec4(aPoint.zw * texOffset.zw + texOffset.xy, texParams.xy);
        #endif

        vColor = uColor[index];
        vParams = vec2(aIndex.y, texParams.y);
    }
}
)";

const std::string SPRITE_FRAGMENT_SOURCE = SHADER_PRECISION +
BLEND_FUNC +
R"(
#define SPRITE_SHADER 1
varying vec4 vColor;
varying vec4 vTexCoord;
varying highp vec2 vParams; // texIndex mulAplha

#ifdef ADJUST_LAYER
uniform sampler2D uSamplerPre;
uniform vec2 uFilterBlend;
vec4 filterMain(vec2 coord,sampler2D tex);
#endif

uniform sampler2D uSampler0;
uniform sampler2D uSampler1;
uniform sampler2D uSampler2;
uniform sampler2D uSampler3;
uniform sampler2D uSampler4;
uniform sampler2D uSampler5;
uniform sampler2D uSampler6;
uniform sampler2D uSampler7;

#if MAX_ITEM_COUNT == 16
uniform sampler2D uSampler8;
uniform sampler2D uSampler9;
uniform sampler2D uSampler10;
uniform sampler2D uSampler11;
uniform sampler2D uSampler12;
uniform sampler2D uSampler13;
uniform sampler2D uSampler14;
uniform sampler2D uSampler15;
#endif

float round(float a){
    return floor(0.5+a);
}

#pragma FILTER_FRAG

vec4 texture2DbyIndex(float index, vec2 coord){
    if (index == 0.){
        return texture2D(uSampler0, coord);
    }
    if (index== 1.){
        return texture2D(uSampler1, coord);
    }
    if (index==2.){
        return texture2D(uSampler2, coord);
    }
    if (index==3.){
        return texture2D(uSampler3, coord);
    }
    if (index==4.){
        return texture2D(uSampler4, coord);
    }
    if (index==5.){
        return texture2D(uSampler5, coord);
    }
    if (index==6.){
        return texture2D(uSampler6, coord);
    }
    if(index == 7.){
        return texture2D(uSampler7, coord);
    }
#if MAX_ITEM_COUNT == 16

    if (index == 8.){
        return texture2D(uSampler8, coord);
    }
    if (index==9.){
        return texture2D(uSampler9, coord);
    }
    if (index==10.){
        return texture2D(uSampler10, coord);
    }
    if (index==11.){
        return texture2D(uSampler11, coord);
    }
    if (index==12.){
        return texture2D(uSampler12, coord);
    }
    if (index==13.){
        return texture2D(uSampler13, coord);
    }
    if (index==14.){
        return texture2D(uSampler14, coord);
    }
    if(index ==15.){
        return texture2D(uSampler15, coord);
    }
#endif
    return texture2D(uSampler0, coord);
}

void main() {
    vec4 color = vec4(0.);
    #ifdef ADJUST_LAYER
        float blend = uFilterBlend.x;
        if (blend >= 1.){
            color = filterMain(vTexCoord.xy,uSamplerPre);
        } else if(blend <= 0.){
            color = texture2D(uSamplerPre,vTexCoord.zw);
        } else {
            color = mix(texture2D(uSamplerPre, vTexCoord.zw), filterMain(vTexCoord.xy,uSamplerPre), blend);
        }
    #else
        vec4 texColor = texture2DbyIndex(round(vParams.x), vTexCoord.xy);
        color = blendColor(texColor,vColor,round(vParams.y));

        if (vTexCoord.z == 0. && color.a < 0.01){
            discard;
        }

    #endif
    gl_FragColor = color;
}
)";


#endif /* sprite_shader_hpp */
