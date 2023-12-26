//
//  particle_shader.hpp
//
//  Created by Zongming Liu on 2022/2/7.
//

#ifndef particle_shader_hpp
#define particle_shader_hpp

#include <string>
#include "shader_defines.h"
#include "value.hpp"
#include "integrate.hpp"
#include "blend.hpp"

// todo: android上可以用 highp float
const std::string PARTICLE_VERTEX_SOURCE = // SHADER_PRECISION +
R"(
#define SHADER_VERTEX 1
#define PATICLE_SHADER 1
)" +
VALUE_FUNC +
INTEGRATE_FUNCTION +
R"(
const float d2r = 3.141592653589793 / 180.;

attribute vec3 aPos;
attribute vec4 aOffset;//texcoord.xy time:start duration
attribute vec3 aVel;
attribute vec3 aRot;
attribute vec4 aColor;
attribute vec3 aDirX;
attribute vec3 aDirY;

#ifdef USE_SPRITE
attribute vec3 aSprite;//start duration cycles
uniform vec4 uSprite;//col row totalFrame blend
vec2 getSpriteUV(vec2 uv, float lifeTime, out vec3 uv1);
varying vec4 vTexCoordBlend;
#endif

#ifdef FINAL_TARGET
uniform vec3 uFinalTarget;
uniform vec4 uForceCurve;
#endif

uniform mat4 uModel;
uniform mat4 uViewP;
uniform mat4 uViewProjection;

uniform vec4 uParams;//time duration endBehavior
uniform vec4 uAcceleration;
uniform vec4 uGravityModifierValue;
uniform vec4 uOpacityOverLifetimeValue;
#ifdef ROT_X_LIFETIME
uniform vec4 uRXByLifeTimeValue;
#endif

#ifdef ROT_Y_LIFETIME
uniform vec4 uRYByLifeTimeValue;
#endif

#ifdef ROT_Z_LIFETIME
uniform vec4 uRZByLifeTimeValue;
#endif

#ifdef COLOR_OVER_LIFETIME
uniform sampler2D uColorOverLifetime;
#endif

#if LINEAR_VEL_X + LINEAR_VEL_Y + LINEAR_VEL_Z
#if LINEAR_VEL_X
uniform vec4 uLinearXByLifetimeValue;
#endif
#if LINEAR_VEL_Y
uniform vec4 uLinearYByLifetimeValue;
#endif
#if LINEAR_VEL_Z
uniform vec4 uLinearZByLifetimeValue;
#endif
#endif

#ifdef SPEED_OVER_LIFETIME
uniform vec4 uSpeedLifetimeValue;
#endif

#if ORB_VEL_X + ORB_VEL_Y + ORB_VEL_Z
#if ORB_VEL_X
uniform vec4 uOrbXByLifetimeValue;
#endif
#if ORB_VEL_Y
uniform vec4 uOrbYByLifetimeValue;
#endif
#if ORB_VEL_Z
uniform vec4 uOrbZByLifetimeValue;
#endif
uniform vec3 uOrbCenter;
#endif

#ifdef SIZE_BY_LIFE
uniform vec4 uSizeByLifetimeValue;
#endif
#ifdef SIZE_Y_BY_LIFE
uniform vec4 uSizeYByLifetimeValue;
#endif
varying float vLife;
varying vec4 vColor;
varying vec2 vTexCoord;

#ifdef USE_FILTER
#pragma FILTER
#endif

vec3 calOrbitalMov(float _life,float _dur){
    vec3 orb = vec3(0.0);
    #ifdef AS_ORBITAL_MOVEMENT
    #define FUNC(a) getValueFromTime(_life,a)
    #else
    #define FUNC(a) getIntegrateFromTime0(_life,a) * _dur
    #endif

    #if ORB_VEL_X
    orb.x = FUNC(uOrbXByLifetimeValue);
    #endif

    #if ORB_VEL_Y
    orb.y = FUNC(uOrbYByLifetimeValue);
    #endif

    #if ORB_VEL_Z
    orb.z = FUNC(uOrbZByLifetimeValue);
    #endif
    #undef FUNC
    return orb;
}

vec3 calLinearMov(float _life,float _dur){
    vec3 mov = vec3(0.0);
    #ifdef AS_LINEAR_MOVEMENT
    #define FUNC(a) getValueFromTime(_life,a)
    #else
    #define FUNC(a) getIntegrateFromTime0(_life,a) * _dur
    #endif

    #if LINEAR_VEL_X
    mov.x = FUNC(uLinearXByLifetimeValue);
    #endif

    #if LINEAR_VEL_Y
    mov.y = FUNC(uLinearYByLifetimeValue);
    #endif

    #if LINEAR_VEL_Z
    mov.z = FUNC(uLinearZByLifetimeValue);
    #endif
    #undef FUNC
    return mov;
}

mat3 mat3FromRotation(vec3 rotation){
    vec3 sinR = sin(rotation * d2r) ;
    vec3 cosR = cos(rotation * d2r) ;
    return mat3(cosR.z, -sinR.z, 0., sinR.z, cosR.z, 0., 0., 0., 1.) *
    mat3(cosR.y, 0., sinR.y, 0., 1., 0., -sinR.y, 0, cosR.y) *
    mat3(1., 0., 0., 0, cosR.x, -sinR.x, 0., sinR.x, cosR.x);
}


    #ifdef USE_SPRITE

vec2 getSpriteUV(vec2 uv, float lifeTime, out vec3 uv1){
    float t = fract(clamp((lifeTime - aSprite.x)/aSprite.y, 0.0, 1.) * aSprite.z);
    float frame = uSprite.z * t;
    float frameIndex = max(ceil(frame) - 1.,0.);
    float row = floor((frameIndex+0.1)/uSprite.x);
    float col = frameIndex - row * uSprite.x;

    vec2 retUV =(vec2(col, row)+uv) / uSprite.xy;

    if(uSprite.w > 0.){
        float blend = frame - frameIndex;
        float frameIndex1 = min(ceil(frame), uSprite.z - 1.);
        float row1 = floor((frameIndex1+0.1)/uSprite.x);
        float col1 = frameIndex1 - row1 * uSprite.x;
        vec2 coord = (vec2(col1, row1)+uv) / uSprite.xy - retUV;
        uv1 = vec3(coord.x,1.- coord.y , blend);
    }
    return vec2(retUV.x,1.- retUV.y);
}
    #endif

vec3 calculateTranslation(vec3 vel, float t0, float t1,float dur){
    float dt = t1 - t0;
    float d = getIntegrateByTimeFromTime(0., dt, uGravityModifierValue);
    vec3 acc = uAcceleration.xyz * d;
    #ifdef SPEED_OVER_LIFETIME
    return vel * getIntegrateFromTime0(dt/dur,uSpeedLifetimeValue) * dur + acc;
    #endif
    return vel * dt + acc;
}

mat3 transformFromRotation(vec3 rot, float time){
    vec3 rotation = rot;
    #ifdef ROT_LIFETIME_AS_MOVEMENT
    #define FUNC1 getValueFromTime
    #else
    #define FUNC1 getIntegrateFromTime0
    #endif

    #ifdef ROT_X_LIFETIME
    rotation.x += FUNC1(time, uRXByLifeTimeValue);
    #endif

    #ifdef ROT_Y_LIFETIME
    rotation.y += FUNC1(time, uRYByLifeTimeValue);
    #endif

    #ifdef ROT_Z_LIFETIME
    rotation.z += FUNC1(time, uRZByLifeTimeValue);
    #endif

    if (dot(rotation, rotation) == 0.0){
        return mat3(1.0);
    }
        #undef FUNC1
    return mat3FromRotation(rotation);
}

void main(){
    float time = uParams.x - aOffset.z;
    float dur = aOffset.w;
    if (time < 0. || time > dur){
        gl_Position = vec4(-3., -3., -3., 1.);
    }
    else {
        float life = clamp(time/dur, 0.0, 1.0);
        vLife = life;

        #ifdef USE_SPRITE
        vec3 tBlend;
        vTexCoord = getSpriteUV(aOffset.xy, time, tBlend);
        vTexCoordBlend = vec4(tBlend,uSprite.w);
        #else
        vTexCoord = aOffset.xy;
        #endif

        vColor = aColor;

        #ifdef COLOR_OVER_LIFETIME
        #ifdef ENABLE_VERTEX_TEXTURE
        vColor *= texture2D(uColorOverLifetime, vec2(life, 0.));
        #endif
        #endif

        vColor.a *= clamp(getValueFromTime(life, uOpacityOverLifetimeValue), 0., 1.);


        vec2 size = vec2(1.0);
        #ifdef SIZE_BY_LIFE
        size = vec2(getValueFromTime(life, uSizeByLifetimeValue));
        #endif

        #ifdef SIZE_Y_BY_LIFE
        size.y = getValueFromTime(life, uSizeYByLifetimeValue);
        #endif

        vec3 point = transformFromRotation(aRot, life) * (aDirX * size.x + aDirY * size.y);
        vec3 pt = calculateTranslation(aVel, aOffset.z, uParams.x,dur);
        vec3 _pos = aPos + pt;

        #if ORB_VEL_X + ORB_VEL_Y + ORB_VEL_Z
        _pos = mat3FromRotation(calOrbitalMov(life,dur))*(_pos - uOrbCenter);
        _pos += uOrbCenter;
        #endif

        #if LINEAR_VEL_X + LINEAR_VEL_Y + LINEAR_VEL_Z
        _pos.xyz += calLinearMov(life,dur);
        #endif

        #ifdef FINAL_TARGET
        float force = getValueFromTime(life,uForceCurve);
        vec4 pos = vec4(mix(_pos,uFinalTarget,force),1.);
        #else
        vec4 pos = vec4(_pos,1.0);
        #endif

        #if RENDER_MODE == 1
        pos.xyz += point;
        pos = uModel * pos;
        #elif RENDER_MODE == 3
        pos = uModel * pos;
        pos.xyz += uViewP[0].xyz * point.x + uViewP[2].xyz * point.y;
        #elif RENDER_MODE == 2
        pos = uModel * pos;
        pos.xy += point.xy;
        #elif RENDER_MODE == 0
        pos = uModel * pos;
        pos.xyz += uViewP[0].xyz * point.x + uViewP[1].xyz * point.y;
        #endif
        gl_Position = uViewProjection * pos;
        vSeed = aSeed;

        gl_PointSize = 6.0;

        #ifdef USE_FILTER
        main_filter(life);
        #endif
    }
}

)";

const std::string PARTICLE_FRAGMENT_SOURCE = //SHADER_PRECISION +
BLEND_FUNC +
R"(
#define PATICLE_SHADER 1
varying float vLife;
varying vec2 vTexCoord;
varying vec4 vColor;

uniform sampler2D uMaskTex;
uniform vec4 uColorParams;//mask preMulAlpha xxx alpha_cutout
uniform vec2 uTexOffset;
#ifdef COLOR_OVER_LIFETIME
uniform sampler2D uColorOverLifetime;
#endif

#ifdef USE_SPRITE
varying vec4 vTexCoordBlend;
#ifdef USE_FILTER
uniform vec4 uFSprite;
#endif
#endif
varying float vSeed;

#ifdef USE_SPRITE
vec4 getTextureColor(sampler2D tex, vec2 texCoord){
    if (vTexCoordBlend.w > 0.){
        return mix(texture2D(tex, texCoord), texture2D(tex, vTexCoordBlend.xy + texCoord), vTexCoordBlend.z);
    }
    return texture2D(tex, texCoord);
}
    #else
    #define getTextureColor texture2D
    #endif


    #ifdef USE_FILTER
    #pragma FILTER
    #endif

float round(float a){
    return floor(0.5+a);
}

void main(){
    vec4 color = vec4(1.0);
    vec4 tempColor = vColor;
    vec2 texOffset = uTexOffset;
    if(vLife < 0.){
        discard;
    }
    #ifdef USE_FILTER
        #ifdef USE_SPRITE
        texOffset = uTexOffset / uFSprite.xy;
        #endif
    color = main_filter(Particle(vTexCoord, texOffset, vLife, vSeed),uMaskTex);
    #else
    if (uColorParams.x > 0.0){
        color = getTextureColor(uMaskTex, vTexCoord);
    }
        #endif

        #ifdef COLOR_OVER_LIFETIME
        #ifndef ENABLE_VERTEX_TEXTURE
        tempColor *= texture2D(uColorOverLifetime, vec2(vLife, 0.));
        #endif
        #endif
    color = blendColor(color,tempColor,round(uColorParams.y));
    if(color.a <= 0.01 && uColorParams.w > 0.){
        float _at = texture2D(uMaskTex, vTexCoord + texOffset ).a + texture2D(uMaskTex, vTexCoord + texOffset * -1.).a;
        if(_at <= 0.02){
            discard;
        }
    }
    gl_FragColor = color;
}

)";

#endif /* particle_shader_hpp */
