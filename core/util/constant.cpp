//
//  constant.cpp
//
//  Created by Zongming Liu on 2022/1/18.
//

#include "constant.hpp"

namespace mn {

PARTICLE_ORIGIN_TYPE CreateParticleOriginType(int value) {
    if (value == 0) {
        return PARTICLE_ORIGIN_TYPE::PARTICLE_ORIGIN_CENTER;
    } else if (value == 1) {
        return PARTICLE_ORIGIN_TYPE::PARTICLE_ORIGIN_LEFT_TOP;
    } else if (value == 2) {
        return PARTICLE_ORIGIN_TYPE::PARTICLE_ORIGIN_LEFT_CENTER;
    } else if (value == 3) {
        return PARTICLE_ORIGIN_TYPE::PARTICLE_ORIGIN_LEFT_BOTTOM;
    } else if (value == 4) {
        return PARTICLE_ORIGIN_TYPE::PARTICLE_ORIGIN_CENTER_TOP;
    } else if (value == 5) {
        return PARTICLE_ORIGIN_TYPE::PARTICLE_ORIGIN_CENTER_BOTTOM;
    } else if (value == 6) {
        return PARTICLE_ORIGIN_TYPE::PARTICLE_ORIGIN_RIGHT_TOP;
    } else if (value == 7) {
        return PARTICLE_ORIGIN_TYPE::PARTICLE_ORIGIN_RIGHT_CENTER;
    } else if (value == 8) {
        return PARTICLE_ORIGIN_TYPE::PARTICLE_ORIGIN_RIGHT_BOTTOM;
    } else {
        return PARTICLE_ORIGIN_TYPE::PARTICLE_ORIGIN_CENTER;
    }
}

RENDER_MODE_TYPE CreateRenderModeType(int value) {
    if (value == 0) {
        return RENDER_MODE_TYPE::RENDER_MODE_BILLBOARD;
    } else if (value == 1) {
        return RENDER_MODE_TYPE::RENDER_MODE_MESH;
    } else if (value == 2) {
        return RENDER_MODE_TYPE::RENDER_MODE_VERTICAL_BILLBOARD;
    } else if (value == 3) {
        return RENDER_MODE_TYPE::RENDER_MODE_HORIZONTAL_BILLBOARD;
    } else {
        return RENDER_MODE_TYPE::RENDER_MODE_BILLBOARD;
    }
}

BLEND_MODE_TYPE CreateBlendModeType(int value) {
    if (value == 0) {
        return BLEND_MODE_TYPE::BLEND_MODE_ALPHA;
    } else if (value == 1) {
        return BLEND_MODE_TYPE::BLEND_MODE_ADDITIVE;
    } else if (value == 2) {
        return BLEND_MODE_TYPE::BLEND_MODE_MULTIPLY;
    } else if (value == 3) {
        return BLEND_MODE_TYPE::BLEND_MODE_LUMINANCE_ALPHA;
    } else if (value == 4) {
        return BLEND_MODE_TYPE::BLEND_MODE_SUBTRACT;
    } else if (value == 5) {
        return BLEND_MODE_TYPE::BLEND_MODE_ADD_LIGHT;
    } else if (value == 6) {
        return BLEND_MODE_TYPE::BLEND_MODE_LIGHT;
    } else if (value == 7) {
        return BLEND_MODE_TYPE::BLEND_MODE_LUMINANCE_ADDITIVE;
    } else {
        return BLEND_MODE_TYPE::BLEND_MODE_ALPHA;
    }
}


MASK_MODE_TYPE CreateMaskModeType(int value) {
    if (value == 0) {
        return MASK_MODE_NONE;
    } else if (value == 1) {
        return MASK_MODE_WRITE_MASK;
    } else if (value == 2) {
        return MASK_MODE_READ_MASK;
    } else if (value == 3) {
        return MASK_MODE_READ_INVERT_MASK;
    } else {
        return MASK_MODE_NONE;
    }
}

END_BEHAVIOR_TYPE CreateEndBehaviorType(int value) {
    if (value == 0) {
        return END_BEHAVIOR_TYPE::END_BEHAVIOR_DESTROY;
    } else if (value == 1) {
        return END_BEHAVIOR_TYPE::END_BEHAVIOR_PAUSE;
    } else if (value == 2) {
        return END_BEHAVIOR_TYPE::END_BEHAVIOR_FORWARD;
    } else if (value == 3) {
        return END_BEHAVIOR_TYPE::END_BEHAVIOR_PAUSE_AND_DESTROY;
    } else if (value == 4) {
        return END_BEHAVIOR_TYPE::END_BEHAVIOR_FREEZE;
    } else if (value == 5) {
        return END_BEHAVIOR_TYPE::END_BEHAVIOR_RESTART;
    } else if (value == 6) {
        return END_BEHAVIOR_TYPE::END_BEHAVIOR_DESTROY_CHILDREN;
    } else {
        return END_BEHAVIOR_TYPE::END_BEHAVIOR_DESTROY;
    }
}

ARC_MODE_TYPE CreateArcModeType(int value) {
    if (value == 0) {
        return ARC_MODE_TYPE::ARC_MODE_RANDOM;
    } else if (value == 1) {
        return ARC_MODE_TYPE::ARC_MODE_LOOP;
    } else if (value == 2) {
        return ARC_MODE_TYPE::ARC_MODE_PINGPONG;
    } else if (value == 3) {
        return ARC_MODE_TYPE::ARC_MODE_BURST_SPREAD;
    } else {
        return ARC_MODE_TYPE::ARC_MODE_RANDOM;
    }
}

RENDER_LEVEL CreateRenderLevel(const char* value) {
    if (!value) {
        return RENDER_LEVEL_B_PLUS;
    }
    if (value[0] == 'S') {
        return RENDER_LEVEL_S;
    }
    if (value[0] == 'A') {
        if (value[1] == '+') {
            return RENDER_LEVEL_A_PLUS;
        }
        return RENDER_LEVEL_A;
    }
    if (value[0] == 'B' && value[1] != '+') {
        return RENDER_LEVEL_B;
    }
    return RENDER_LEVEL_B_PLUS;
}

SIDE_MODE_TYPE CreateSideModeType(int value) {
    if (value == 1032) {
        return SIDE_MODE_TYPE::SIDE_BOTH;
    } else if (value == 1028) {
        return SIDE_MODE_TYPE::SIDE_FRONT;
    } else if (value == 1029) {
        return SIDE_MODE_TYPE::SIDE_BACK;
    } else {
        return SIDE_MODE_TYPE::SIDE_BOTH;
    }
}

}
