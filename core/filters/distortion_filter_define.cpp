//
//  distortion_filter_define.cpp
//  MarsNative
//
//  Created by Zongming Liu on 2022/8/2.
//  Copyright © 2022 Alipay. All rights reserved.
//

#include "distortion_filter_define.hpp"
#include "shader/adjust/distortion_frag.h"
#include "shader/adjust/distortion_vert.h"
#include "math/value/value_getter.hpp"
#include "util/vfx_value_getter.h"
#include <cmath>

namespace mn {

DistortionFilterDefine::DistortionFilterDefine(Composition* composition, DistortionFilterData* filter_data) : FilterDefine(composition, filter_data) {
    mesh_filter_option_.fragment = DISTORTION_FRAG;
    mesh_filter_option_.shader_cache_id = "distortion";
    
    center_[0] = filter_data->center[0];
    center_[1] = filter_data->center[1];
    
    direction_[0] = filter_data->direction[0];
    direction_[1] = filter_data->direction[1];
    
    u_period_value_ = VFXValueGetter::CreateValueGetter(filter_data->period);
    u_strength_value_ = VFXValueGetter::CreateValueGetter(filter_data->strength);
    u_movement_value_ = VFXValueGetter::CreateValueGetter(filter_data->wave_movement);
    
    mesh_filter_option_.material_state.blending = std::make_shared<MBool>(false);
    mesh_filter_option_.material_state.cull_face_enabled = std::make_shared<MBool>(false);
    
    std::shared_ptr<UniformValue> u_wave_params_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
    float wave_data[4] = { center_[0], center_[1], direction_[0], direction_[1] };
    u_wave_params_value->SetData(wave_data, 0, 4);
    mesh_filter_option_.uniform_values.insert({"uWaveParams", u_wave_params_value});
    
    mesh_filter_option_.uniform_variables.push_back("vWaveParams");
    std::shared_ptr<UniformValue> v_wave_params_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
    mesh_filter_option_.uniform_values.insert({"vWaveParams", v_wave_params_value});
}

DistortionFilterDefine::~DistortionFilterDefine() {
    DEBUG_MLOGD("DistortionFilterDefine Destruction");
    
    if (u_period_value_) {
        delete u_period_value_;
    }

    if (u_strength_value_) {
        delete u_strength_value_;
    }
    
    if (u_movement_value_) {
        delete u_movement_value_;
    }
}


std::vector<std::shared_ptr<RenderPass>> DistortionFilterDefine::GeneratePreRenderPasses(std::vector<std::shared_ptr<MarsMesh>> meshes) {
    std::vector<std::shared_ptr<RenderPass>> pre_render_pass;
    return pre_render_pass;
}

void DistortionFilterDefine::GetVariableUniformValue(const std::string& key, float life, std::shared_ptr<UniformValue> value) {
    if (key == "vWaveParams") {
        float period_value = u_period_value_->GetValue(life) * M_PI * 2;
        float move_vlaue = u_movement_value_->GetValue(life) * M_PI * 2;
        float strenth_value = u_strength_value_->GetValue(life);
        
        float v_move_params[4] = { period_value, move_vlaue, strenth_value, 0};
        value->SetData(v_move_params, 0, 4);
    }
}

}

