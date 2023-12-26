//
//  gl_program.cpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#include "gl_program.hpp"
#include "util/log_util.hpp"

#include "ri/backend/opengl/gl_api.hpp"
#include "ri/render/render_frame.hpp"
#include "ri/utils.hpp"
#include "ri/backend/opengl/gl_gpu_renderer.hpp"

namespace mn {

GLProgram::GLProgram(GLGPURenderer *renderer ,GLuint program) : renderer_(renderer), program_(program) {
    empty_texture_ = renderer_->EmptyTexture();
    this->Init();
}

GLProgram::~GLProgram() {
    if (program_ > 0) {
        OpenGLApi::DeleteProgram(program_);
    }
}

void GLProgram::Init() {
    this->Bind();
    this->InitAttributes();
    this->InitUniforms();
    
    this->UnBind();
    this->renderer_->State().ActiveTexture(GL_TEXTURE0);
    
}

void GLProgram::InitAttributes() {
    GLint attrCount = 0;
    GLint attrMaxLength = 0;
    
    OpenGLApi::GetProgramiv(program_, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attrMaxLength);
    OpenGLApi::GetProgramiv(program_, GL_ACTIVE_ATTRIBUTES, &attrCount);
    
    GLchar glName[256];
    GLsizei glLength;
    GLsizei glSize;
    GLenum glType;
    
    for (GLint i=0; i<attrCount; i++) {
        memset(glName, 0, sizeof(glName));
        OpenGLApi::GetActiveAttrib(program_, i, attrMaxLength, &glLength, &glSize, &glType, glName);
        GLint location = OpenGLApi::GetAttribLocation(program_, glName);
        
        std::string name(glName);
        ProgramAttributeInfo attribute_info(name, location, glType, glSize);
        attribute_map_.insert({name, attribute_info});
    }
}

void GLProgram::InitUniforms() {
    GLint uniformCount = 0;
    OpenGLApi::GetProgramiv(program_, GL_ACTIVE_UNIFORMS, &uniformCount);
    
    GLchar glName[256];
    GLsizei glLength;
    GLsizei glSize;
    GLenum glType;
    
    // 多纹理对应positionwen
    int32_t texture_index = 0;
    for (GLint i=0; i<uniformCount; i++) {
        memset(glName, 0, sizeof(glName));
        OpenGLApi::GetActiveUniform(program_, i, 255, &glLength, &glSize, &glType, glName);
        GLint location = OpenGLApi::GetUniformLocation(program_, glName);
        
        // uniform数组会取到uRot[0]这种；需要特殊处理下
        std::string name(glName);
        if (name.find("[0]") != std::string::npos && name.find(".") == std::string::npos) {
            name.replace(name.find("[0]"), 3, "");
        }
        
        ProgramUniformInfo info(name, location, glSize, glType);
        if (glType == GL_SAMPLER_2D || glType == GL_SAMPLER_CUBE) {
            info.texture_index = texture_index;
            
            for (int32_t k=0; k<glSize; k++) {
                OpenGLApi::Uniform1i(location, texture_index + k);
                this->renderer_->State().ActiveTexture(GL_TEXTURE0 + texture_index + k);
                if (glType == GL_SAMPLER_2D) {
                    empty_texture_->Bind();
                } else if (glType == GL_SAMPLER_CUBE) {
                    // todo: sample_cube;
                    MLOGD("not support sampler_cube");
                }
            }
            texture_index += glSize;
        }
        
        uniform_map_.insert({name, info});
    }
    
    this->renderer_->State().ActiveTexture(GL_TEXTURE0);
}

void GLProgram::SetupUniforms(const RenderState& state) {
    this->Bind();
    auto material = state.current_mesh->GetMaterial();
    RenderFrame* render_frame = state.current_render_frame;
    RenderPass* render_pass = state.current_render_pass;
    if (material) {
        for (auto& iter : uniform_map_) {
            std::string name = iter.first;
            ProgramUniformInfo& uniform_info = iter.second;
            
            // todo: 10.2.90 添加frame/pass semantic功能;
            if (material->HasUnifromValue(name)) {
                std::shared_ptr<UniformValue> uniform_value = material->GetUniformValue(name);
                if (uniform_value->is_semantic) {
                    SemanticValue *semantic_value = nullptr;
                    if (render_pass->GetSemanticValue(uniform_value->semantic_name)) {
                        semantic_value = render_pass->GetSemanticValue(uniform_value->semantic_name);
                    } else if (render_frame->GetSemeticValue(uniform_value->semantic_name)) {
                         semantic_value = render_frame->GetSemeticValue(uniform_value->semantic_name);
                    }
                    
                    if (semantic_value) {
                        semantic_value->GetSemantiveValue(state, uniform_value);
                    } else {
                        MLOGE("RenderPass %s, SetUniform SemanticValue Failed: %s", render_pass->Name().c_str(), name.c_str());
                    }
                }
                
                this->SetGLUniformValue(name, uniform_info, uniform_value);
            } else if (uniform_info.type != GL_SAMPLER_2D) {
                MLOGE("SetUniform Faild for %s uniformValue not found", name.c_str());
            }
        }
    } else {
        MLOGE("setup uniform failed for material is null");
    }
}

void GLProgram::SetGLUniformValue(const std::string& name, const ProgramUniformInfo& uniform, std::shared_ptr<UniformValue> uniform_data) {
    DEBUG_MLOGD("SetGLUniformValue name: %s", name.c_str());
    
    if (uniform.type == GL_SAMPLER_2D || uniform.type == GL_SAMPLER_CUBE) {
        if (uniform_data && uniform_data->texture) {
            GLTexture *texture = uniform_data->texture->GetInnerTexture();
            renderer_->State().ActiveTexture(GL_TEXTURE0 + uniform.texture_index);
            if (texture) {
                texture->Bind();
            }
        }
        return ;
    }
    
    if (uniform.type != Utils::ConverUniformDataType(uniform_data->data_type)) {
        MLOGE("setUniformValue failed for data type not match %s", name.data());
        return ;
    }
    
    switch (uniform.type) {
        case GL_INT:
            OpenGLApi::Uniform1iv(uniform.location, uniform.size, reinterpret_cast<const GLint *>(uniform_data->GetData()));
            break;
        case GL_INT_VEC2:
            OpenGLApi::Uniform2iv(uniform.location, uniform.size, reinterpret_cast<const GLint *>(uniform_data->GetData()));
            break;
        case GL_INT_VEC3:
            OpenGLApi::Uniform3iv(uniform.location, uniform.size, reinterpret_cast<const GLint *>(uniform_data->GetData()));
            break;
        case GL_INT_VEC4:
            OpenGLApi::Uniform4iv(uniform.location, uniform.size, reinterpret_cast<const GLint *>(uniform_data->GetData()));
            break;
        case GL_FLOAT:
            OpenGLApi::Uniform1fv(uniform.location, uniform.size, reinterpret_cast<const GLfloat *>(uniform_data->GetData()));
            break;
        case GL_FLOAT_VEC2:
            OpenGLApi::Uniform2fv(uniform.location, uniform.size, reinterpret_cast<const GLfloat *>(uniform_data->GetData()));
            break;
        case GL_FLOAT_VEC3:
            OpenGLApi::Uniform3fv(uniform.location, uniform.size, reinterpret_cast<const GLfloat *>(uniform_data->GetData()));
            break;
        case GL_FLOAT_VEC4:
            OpenGLApi::Uniform4fv(uniform.location, uniform.size, reinterpret_cast<const GLfloat *>(uniform_data->GetData()));
            break;
        case GL_FLOAT_MAT2:
            OpenGLApi::UniformMatrix2fv(uniform.location, uniform.size, GL_FALSE, reinterpret_cast<const GLfloat *>(uniform_data->GetData()));
            break;
        case GL_FLOAT_MAT3:
            OpenGLApi::UniformMatrix3fv(uniform.location, uniform.size, GL_FALSE, reinterpret_cast<const GLfloat *>(uniform_data->GetData()));
            break;
        case GL_FLOAT_MAT4:
            OpenGLApi::UniformMatrix4fv(uniform.location, uniform.size, GL_FALSE, reinterpret_cast<const GLfloat *>(uniform_data->GetData()));
            break;
        default:
            MLOGE("Not Support GLType");
            break;
    }
}

void GLProgram::SetupAttributes(GLGeometry *geometry) {
    for (auto& iter : attribute_map_) {
        std::string name = iter.first;
        ProgramAttributeInfo& attribute_info = iter.second;
        AttributeBase *attribute = geometry->GetAttributeBase(name);
        if (attribute) {
            std::string data_source = attribute->data_source;
            auto buffer = geometry->GetAttributeBuffer(data_source);
            if (buffer && attribute) {
                buffer->Bind();
                OpenGLApi::EnableVertexAttribArray(attribute_info.location);
                OpenGLApi::VertexAttribPointer(attribute_info.location, attribute->size, attribute->type, GL_FALSE, attribute->stride, (const void *)attribute->offset);
            }
        } else {
            MLOGE("Set Attribute Failed attribute is null");
        }
    }
}

void GLProgram::Bind() {
    OpenGLApi::UseProgram(program_);
}

void GLProgram::UnBind() {
    OpenGLApi::UseProgram(NULL);
}

}
