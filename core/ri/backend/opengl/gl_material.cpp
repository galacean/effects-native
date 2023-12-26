//
//  gl_material.cpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#include "gl_material.hpp"

namespace mn {

GLMaterial::GLMaterial(GLGPURenderer *renderer, std::shared_ptr<MaterialRenderStates> material_render_state,
                       const std::string& cache_id) : renderer_(renderer), states_(material_render_state), shader_cache_id_(cache_id) {
//    MLOGD("GLMaterial created with shaderId %s", shader_cache_id_.data());
}

GLMaterial::~GLMaterial() {
    
}

void GLMaterial::SetUpStates() {
    auto& webgl_state = renderer_->State();
    if (states_->sample_alpha_to_coverage->val) {
        webgl_state.Enable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    } else {
        webgl_state.Disable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    }
    if (states_->blending->val) {
        webgl_state.Enable(GL_BLEND);
    } else {
        webgl_state.Disable(GL_BLEND);
    }
    if (states_->depth_test->val) {
        webgl_state.Enable(GL_DEPTH_TEST);
    } else {
        webgl_state.Disable(GL_DEPTH_TEST);
    }
    if (states_->stencil_test->val) {
        webgl_state.Enable(GL_STENCIL_TEST);
    } else {
        webgl_state.Disable(GL_STENCIL_TEST);
    }
    if (states_->cull_face_enabled->val) {
        webgl_state.Enable(GL_CULL_FACE);
        webgl_state.CullFace((GLenum)(states_->cull_face->val));
        webgl_state.FrontFace((GLenum)(states_->front_face->val));
    } else {
        webgl_state.Disable(GL_CULL_FACE);
    }
    
    if (states_->stencil_test && states_->stencil_test->val) {
        webgl_state.StencilMaskSeparate(GL_BACK, states_->stencil_mask_back->val);
        webgl_state.StencilMaskSeparate(GL_FRONT, states_->stencil_mask_front->val);
        webgl_state.StencilFuncSeparate(GL_BACK, states_->stencil_func_back->func, states_->stencil_func_back->ref, states_->stencil_func_back->mask);
        webgl_state.StencilFuncSeparate(GL_FRONT, states_->stencil_func_front->func, states_->stencil_func_front->ref, states_->stencil_func_front->mask);
        webgl_state.StencilOpSeparate(GL_BACK, states_->stencil_op_back->fail, states_->stencil_op_back->zfail, states_->stencil_op_back->zpass);
        webgl_state.StencilOpSeparate(GL_FRONT, states_->stencil_op_front->fail, states_->stencil_op_front->zfail, states_->stencil_op_front->zpass);
    }
    
    if (states_->blending && states_->blending->val) {
        webgl_state.BlendColor(states_->blend_color->m[0], states_->blend_color->m[1], states_->blend_color->m[2], states_->blend_color->m[3]);
        webgl_state.BlendEquationSeparate(states_->blend_equation_rgb->val, states_->blend_equation_alpha->val);
        webgl_state.BlendFuncSeparate(states_->blend_src->val, states_->blend_dst->val, states_->blend_src_alpha->val, states_->blend_dst_alpha->val);
    }
    
    webgl_state.ColorMask(states_->color_mask->m[0], states_->color_mask->m[1], states_->color_mask->m[2], states_->color_mask->m[3]);
    
    if (states_->depth_test && states_->depth_test->val) {
        webgl_state.DepthMask(states_->depth_mask->val);
        webgl_state.DepthFunc(states_->depth_func->val);
        webgl_state.DepthRangef(states_->depth_range_znear->val, states_->depth_range_zfar->val);
    }
    
    if (states_->cull_face_enabled && states_->cull_face_enabled->val) {
        webgl_state.CullFace(states_->cull_face->val);
        webgl_state.FrontFace(states_->front_face->val);
    }
}

GLProgram* GLMaterial::GetProgram() {
    DEBUG_MLOGD("GetProgram shader_cache_id_ %s", shader_cache_id_.c_str());
    GLShaderLibrary& shader_library = renderer_->ShaderLibrary();
    return shader_library.GetProgram(shader_cache_id_);
}

}
