//
//  mars_material.cpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#include "mars_material.hpp"
#include "ri/render/mars_renderer.hpp"
#include "util/log_util.hpp"

namespace mn {

static int MARS_MATERIAL_SEED = 0;

MarsMaterial::MarsMaterial(MarsRenderer *renderer, const MaterialOptions& material_option) :
                shader_(material_option.shader), shader_cached_id_(material_option.shader_cached_id),
                name_(material_option.name), renderer_(nullptr), inner_material_(nullptr) {
                    
    states_ = CreateMaterialStates(*material_option.states.get());
    if (renderer) {
        this->AssignRenderer(renderer);
    }
}

MarsMaterial::MarsMaterial(MarsRenderer* renderer, const Shader& shader, const MaterialRenderStates& states, const std::string& name) :shader_(shader), renderer_(nullptr), inner_material_(nullptr) {
    
    if (name == "Material") {
        name_ = name + std::to_string(MARS_MATERIAL_SEED++);
    } else {
        name_ = name;
    }
    states_ = CreateMaterialStates(states);
    if (renderer) {
        this->AssignRenderer(renderer);
    }
    DEBUG_MLOGD("MarsMaterial name %s Construct", name_.data());

}

MarsMaterial::~MarsMaterial() {
    DEBUG_MLOGD("MarsMaterial name %s Destruct", name_.data());
    if (inner_material_) {
        delete inner_material_;
    }
}

void MarsMaterial::AssignRenderer(MarsRenderer *renderer) {
    if (!inner_material_) {
        renderer_ = renderer;
        GLShaderLibrary& shader_library = renderer_->InnerRenderer()->ShaderLibrary();
        
//        this->shader_cached_id_ = shader_library.AddShader(shader_, macros_, this->shader_cached_id_);
        this->shader_cached_id_ = shader_library.AddShader(shader_);
        ShaderCompileResult result = shader_library.CompileShader(this->shader_cached_id_);
        if (result == ShaderCompileResult::SUCCESS) {
            inner_material_ = new GLMaterial(renderer_->InnerRenderer(), states_, this->shader_cached_id_);
        } else {
            MLOGE("Compile Shader Failed");
        }
        
        for (auto iter : uniform_values_) {
            this->AssignUniformValueRenderer(iter.second);
        }
    }
}

bool MarsMaterial::HasUnifromValue(const std::string& name) {
    if (uniform_values_.find(name) != uniform_values_.end()) {
        return true;
    }
    
    return false;
}

std::shared_ptr<UniformValue> MarsMaterial::GetUniformValue(const std::string& name) {
    if (this->HasUnifromValue(name)) {
        return uniform_values_[name];
    }
    return nullptr;
}


std::shared_ptr<UniformValue> MarsMaterial::CreateUniformValueIfNot(const std::string& name, UniformDataType data_type) {
    if (this->HasUnifromValue(name)) {
        auto uniform_value = uniform_values_[name];
        if (uniform_value->data_type != data_type) {
            assert(0);
        }
        return uniform_value;
    } else {
        std::shared_ptr<UniformValue> uniform_value = std::make_shared<UniformValue>(data_type);
        uniform_values_[name] = uniform_value;
        return uniform_value;
    }
}


std::unordered_map<std::string, std::shared_ptr<UniformValue>>& MarsMaterial::GetUniformValues() {
    return uniform_values_;
}

void MarsMaterial::SetUniformValue(const std::string& name, std::shared_ptr<UniformValue> value) {
    if (value) {
        uniform_values_[name] = value;
        this->AssignUniformValueRenderer(value);
    } else {
        uniform_values_.erase(name);
    }
}

void MarsMaterial::SetUniformValues(const std::unordered_map<std::string, std::shared_ptr<UniformValue>> uniform_values) {
    for (auto& iter : uniform_values) {
        const std::string& name = iter.first;
        auto uniform_value = iter.second;
        if (uniform_value) {
            uniform_values_.insert({ name, uniform_value });
            this->AssignUniformValueRenderer(uniform_value);
        } else {
            uniform_values_.erase(name);
        }
    }
}

void MarsMaterial::AssignUniformValueRenderer(std::shared_ptr<UniformValue> value) {
    if (renderer_) {
        if (value->texture) {
            value->texture->AssignRenderer(renderer_);
        }
    }
}

std::shared_ptr<MaterialRenderStates> MarsMaterial::CreateMaterialStates(const MaterialRenderStates& states) {
    // todo: 优化
    std::shared_ptr<MaterialRenderStates> ret = std::make_shared<MaterialRenderStates>();
    
    ret->blending = std::make_shared<MBool>(states.blending ? states.blending->val : false);
    ret->sample_alpha_to_coverage = std::make_shared<MBool>(states.sample_alpha_to_coverage ? states.sample_alpha_to_coverage->val : false);
    ret->depth_test = std::make_shared<MBool>(states.depth_test ? states.depth_test->val : false);
    ret->stencil_test = std::make_shared<MBool>(states.stencil_test ? states.stencil_test->val : false);
    ret->cull_face_enabled = std::make_shared<MBool>(states.cull_face_enabled ? states.cull_face_enabled->val : false);
    
    if (!states.stencil_mask) {
        ret->stencil_mask_back = std::make_shared<MUint>(states.stencil_mask_back ? states.stencil_mask_back->val : 0xFF);
        ret->stencil_mask_front = std::make_shared<MUint>(states.stencil_mask_front ? states.stencil_mask_front->val : 0xFF);
    } else {
        auto stencil_mask_back = states.stencil_mask_back ? states.stencil_mask_back : states.stencil_mask;
        ret->stencil_mask_back = std::make_shared<MUint>(stencil_mask_back ? stencil_mask_back->val : 0xFF);
        auto stencil_mask_front = states.stencil_mask_front ? states.stencil_mask_front : states.stencil_mask;
        ret->stencil_mask_front = std::make_shared<MUint>(stencil_mask_front ? stencil_mask_front->val : 0xFF);
    }
    
    ret->stencil_func_back = std::make_shared<RENDER_STATE_STENCIL_FUNC>();
    if (states.stencil_func_back) {
        ret->stencil_func_back->func = states.stencil_func_back->func;
        ret->stencil_func_back->ref = states.stencil_func_back->ref;
        ret->stencil_func_back->mask = states.stencil_func_back->mask;
    } else if (states.stencil_func) {
        ret->stencil_func_back->func = states.stencil_func->func;
        ret->stencil_func_back->ref = states.stencil_func->ref;
        ret->stencil_func_back->mask = states.stencil_func->mask;
    } else {
        ret->stencil_func_back->func = GL_ALWAYS;
        ret->stencil_func_back->ref = 0;
        ret->stencil_func_back->mask = 0xFF;
    }
    ret->stencil_func_front = std::make_shared<RENDER_STATE_STENCIL_FUNC>();
    if (states.stencil_func_front) {
        ret->stencil_func_front->func = states.stencil_func_front->func;
        ret->stencil_func_front->ref = states.stencil_func_front->ref;
        ret->stencil_func_front->mask = states.stencil_func_front->mask;
    } else if (states.stencil_func) {
        ret->stencil_func_front->func = states.stencil_func->func;
        ret->stencil_func_front->ref = states.stencil_func->ref;
        ret->stencil_func_front->mask = states.stencil_func->mask;
    } else {
        ret->stencil_func_front->func = GL_ALWAYS;
        ret->stencil_func_front->ref = 0;
        ret->stencil_func_front->mask = 0xFF;
    }
    
    ret->stencil_op_front = std::make_shared<RENDER_STATE_STENCIL_OP>();
    if (states.stencil_op_front) {
        ret->stencil_op_front->fail = states.stencil_op_front->fail;
        ret->stencil_op_front->zfail = states.stencil_op_front->zfail;
        ret->stencil_op_front->zpass = states.stencil_op_front->zpass;
    } else if (states.stencil_op) {
        ret->stencil_op_front->fail = states.stencil_op->fail;
        ret->stencil_op_front->zfail = states.stencil_op->zfail;
        ret->stencil_op_front->zpass = states.stencil_op->zpass;
    } else {
        ret->stencil_op_front->fail = GL_KEEP;
        ret->stencil_op_front->zfail = GL_KEEP;
        ret->stencil_op_front->zpass = GL_KEEP;
    }
    ret->stencil_op_back = std::make_shared<RENDER_STATE_STENCIL_OP>();
    if (states.stencil_op_back) {
        ret->stencil_op_back->fail = states.stencil_op_back->fail;
        ret->stencil_op_back->zfail = states.stencil_op_back->zfail;
        ret->stencil_op_back->zpass = states.stencil_op_back->zpass;
    } else if (states.stencil_op) {
        ret->stencil_op_back->fail = states.stencil_op->fail;
        ret->stencil_op_back->zfail = states.stencil_op->zfail;
        ret->stencil_op_back->zpass = states.stencil_op->zpass;
    } else {
        ret->stencil_op_back->fail = GL_KEEP;
        ret->stencil_op_back->zfail = GL_KEEP;
        ret->stencil_op_back->zpass = GL_KEEP;
    }
    
    //blend
    ret->blend_src = std::make_shared<MUint>(states.blend_src ? states.blend_src->val : GL_ONE);
    ret->blend_src_alpha = std::make_shared<MUint>(states.blend_src_alpha ? states.blend_src_alpha->val : ret->blend_src->val);
    ret->blend_dst = std::make_shared<MUint>(states.blend_dst ? states.blend_dst->val : GL_ZERO);
    ret->blend_dst_alpha = std::make_shared<MUint>(states.blend_dst_alpha ? states.blend_dst_alpha->val : ret->blend_dst->val);
    ret->blend_equation_rgb = std::make_shared<MUint>(states.blend_equation_rgb ? states.blend_equation_rgb->val : GL_FUNC_ADD);
    ret->blend_equation_alpha = std::make_shared<MUint>(states.blend_equation_alpha ? states.blend_equation_alpha->val : ret->blend_equation_rgb->val);
    ret->blend_color = std::make_shared<Vec4>();
    if (states.blend_color) {
        ret->blend_color->Set(*states.blend_color.get());
    } else {
        ret->blend_color->Set(0, 0, 0, 0);
    }
    
    //color
    ret->color_mask = std::make_shared<Vec4>();
    if (states.color_mask) {
        ret->color_mask->Set(*states.color_mask.get());
    } else {
        ret->color_mask->Set(1, 1, 1, 1);
    }
    
    //depth
    ret->depth_mask = std::make_shared<MBool>(states.depth_mask ? states.depth_mask->val : true);
    ret->depth_func = std::make_shared<MFloat>(states.depth_func ? states.depth_func->val : GL_LESS);
    ret->depth_range_znear = std::make_shared<MFloat>(states.depth_range_znear ? states.depth_range_znear->val : 0);
    ret->depth_range_zfar = std::make_shared<MFloat>(states.depth_range_zfar ? states.depth_range_zfar->val : 1);
    ret->cull_face = std::make_shared<MUint>(states.cull_face ? states.cull_face->val : GL_BACK);
    ret->front_face = std::make_shared<MUint>(states.front_face ? states.front_face->val : GL_CCW);
    
    return ret;
}

}
