//
//  translate.cpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/4/12.
//

#include "translate.h"
#include "plugin/sprite/sprite_item.h"
#include "math/math_util.hpp"

namespace mn {

void Translate::CalculateTranslation(Vec3& out, TranslateTarget* target, const Vec3& acc, float time, float duration, Vec3* pos_data, Vec3* vel_data, int pos_start_index, int vel_start_index) {
    const float lifetime = time / duration;
    float speed_integrate = time;
    const auto& speed_over_lifetime = target->GetSpeedOverLifetime();
    if (speed_over_lifetime) {
        speed_integrate = speed_over_lifetime->GetIntegrateValue(0, time, duration);
    }
    float d = target->GetGravityModifier() ? target->GetGravityModifier()->GetIntegrateByTime(0, time) : 0;
    for (int i = 0; i < 3; i++) {
        out.m[i] = pos_data[pos_start_index / 3].m[i] + vel_data[vel_start_index / 3].m[i] * speed_integrate + acc.m[i] * d;
    }
    const auto& linear_vel_over_lifetime = target->GetLinearVelOverLifetime();
    const auto& orb_vel_over_lifetime = target->GetOrbitalVelOverLifetime();
    if (orb_vel_over_lifetime && orb_vel_over_lifetime->enabled) {
        Vec3 center;
        if (orb_vel_over_lifetime->center) {
            center.Set(*orb_vel_over_lifetime->center);
        }
        const Vec3 pos(out.m[0] - center.m[0], out.m[1] - center.m[1], out.m[2] - center.m[2]);
        const bool as_rotation = orb_vel_over_lifetime->as_rotation;
        Vec3 rotation;
        if (orb_vel_over_lifetime->x) {
            rotation.m[0] = as_rotation ? orb_vel_over_lifetime->x->GetValue(lifetime) : orb_vel_over_lifetime->x->GetIntegrateValue(0, time, duration);
        }
        if (orb_vel_over_lifetime->y) {
            rotation.m[1] = as_rotation ? orb_vel_over_lifetime->y->GetValue(lifetime) : orb_vel_over_lifetime->y->GetIntegrateValue(0, time, duration);
        }
        if (orb_vel_over_lifetime->z) {
            rotation.m[2] = as_rotation ? orb_vel_over_lifetime->z->GetValue(lifetime) : orb_vel_over_lifetime->z->GetIntegrateValue(0, time, duration);
        }
        Mat3 rot_mat3;
        MathUtil::Mat3FromRotation(rot_mat3, rotation);
        Vec3 rot;
        MathUtil::Vec3MulMat3(rot, pos, rot_mat3);
        rot.Add(center);
        out.Set(rot);
    }
    if (linear_vel_over_lifetime && linear_vel_over_lifetime->enabled) {
        const bool as_movement = linear_vel_over_lifetime->as_movement;
        if (linear_vel_over_lifetime->x) {
            out.m[0] += as_movement ? linear_vel_over_lifetime->x->GetValue(lifetime) : linear_vel_over_lifetime->x->GetIntegrateValue(0, time, duration);
        }
        if (linear_vel_over_lifetime->y) {
            out.m[1] += as_movement ? linear_vel_over_lifetime->y->GetValue(lifetime) : linear_vel_over_lifetime->y->GetIntegrateValue(0, time, duration);
        }
        if (linear_vel_over_lifetime->z) {
            out.m[2] += as_movement ? linear_vel_over_lifetime->z->GetValue(lifetime) : linear_vel_over_lifetime->z->GetIntegrateValue(0, time, duration);
        }
    }
}

void Translate::ParticleOriginMap(float *origin_data, PARTICLE_ORIGIN_TYPE type) {
    float x = 0.0;
    float y = 0.0;
    float origin[8] = {-0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, -0.5};

    ParticleOriginPointData(x, y, type);
    
    for (size_t i = 0; i < 8; i += 2) {
        origin_data[i] = origin[i] + x;
        origin_data[i + 1] = origin[i + 1] + y;
    }
}

void Translate::ParticleOriginPointData(float &x, float &y, PARTICLE_ORIGIN_TYPE type) {
    switch (type) {
        case PARTICLE_ORIGIN_CENTER:
            x = 0.0;
            y = 0.0;
            break;
        case PARTICLE_ORIGIN_CENTER_TOP:
            x = 0.0;
            y = -0.5;
            break;
        case PARTICLE_ORIGIN_CENTER_BOTTOM:
            x = 0.0;
            y = 0.5;
            break;
        case PARTICLE_ORIGIN_LEFT_TOP:
            x = 0.5;
            y = -0.5;
            break;
        case PARTICLE_ORIGIN_LEFT_CENTER:
            x = 0.5;
            y = 0.0;
            break;
        case PARTICLE_ORIGIN_LEFT_BOTTOM:
            x = 0.5;
            y = 0.5;
            break;
        case PARTICLE_ORIGIN_RIGHT_TOP:
            x = -0.5;
            y = -0.5;
            break;
        case PARTICLE_ORIGIN_RIGHT_CENTER:
            x = -0.5;
            y = 0.0;
            break;
        case PARTICLE_ORIGIN_RIGHT_BOTTOM:
            x = -0.5;
            y = 0.5;
            break;
        default:
            break;
    }
}

void Translate::ConvertParticleOrigin2Anchor(Vec2* anchor, PARTICLE_ORIGIN_TYPE particle_origin) {
    if (!anchor) {
        MLOGE("ConvertParticleOrigin2Anchor anchor is null");
        return;
    }
    float x = 0, y = 0;
    ParticleOriginPointData(x, y, particle_origin);
    anchor->Set(0.5f - x, y + 0.5f);
}

}
