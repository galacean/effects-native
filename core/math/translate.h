#ifndef MN_MATH_TRANSLATE_H_
#define MN_MATH_TRANSLATE_H_

#include "util/constant.hpp"
#include "math/mat3.hpp"
#include "math/vec3.hpp"
#include "math/vec4.hpp"
#include "math/value/value_getter.hpp"
#include "plugin/types.h"

namespace mn {

class TranslateTarget {
public:
    TranslateTarget() {}
    
    virtual ~TranslateTarget() {}

    virtual ValueGetter* GetSpeedOverLifetime() = 0;
    
    virtual ValueGetter* GetGravityModifier() = 0;
    
    virtual LinearVelOverLifetime* GetLinearVelOverLifetime() = 0;
    
    virtual OrbitalVelOverLifetime* GetOrbitalVelOverLifetime() = 0;
};

class Translate {
    
public:
    
    static void CalculateTranslation(Vec3& out, TranslateTarget* target, const Vec3& acc, float time,
        float duration, Vec3* pos_data, Vec3* vel_data, int pos_start_index = 0, int vel_start_index = 0);
    
    static void ParticleOriginMap(float *origin_data, PARTICLE_ORIGIN_TYPE type);
    
    static void ParticleOriginPointData(float &x, float &y, PARTICLE_ORIGIN_TYPE type);
    
    static void ConvertParticleOrigin2Anchor(Vec2* anchor, PARTICLE_ORIGIN_TYPE particle_origin);
    
};

}

#endif
