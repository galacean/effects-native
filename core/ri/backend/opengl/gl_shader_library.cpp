//
//  gl_shader_library.cpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#include <sstream>

#include "gl_shader_library.hpp"
#include "util/log_util.hpp"
#include "ri/backend/opengl/gl_api.hpp"

namespace mn {

static int s_shader_seed = 0;

GLShaderLibrary::GLShaderLibrary(GLGPURenderer *renderer) : renderer_(renderer) {
    DEBUG_MLOGD("GLShader Library Created");
}

GLShaderLibrary::~GLShaderLibrary() {
    DEBUG_MLOGD("GLShader Library Destruct");
    for (auto iter : program_map_) {
        GLProgram *program = iter.second;
        if (program) {
            const std::string& name = iter.first;
            DEBUG_MLOGD("GLProgram Delete %s", name.data());
            delete program;
        }
    }
}

std::string GLShaderLibrary::AddShader(Shader& shader) {
    std::string macro_head = this->CombineMacros(shader.macros);
    
    std::string vs = macro_head + shader.vertex;
    std::string fs = macro_head + shader.fragment;
    
    std::string shader_cache_id;
    bool shared = false;
    if (shader.shaderd || !shader.cache_id.empty()) {
        shader_cache_id = !shader.cache_id.empty() ? shader.cache_id : this->StrHashCode(vs, fs);
        shared = true;
    } else {
        shader_cache_id = "instanced_" + std::to_string(s_shader_seed++);
    }
    
    if (shader_map_.find(shader_cache_id) == shader_map_.end()) {
        shader.vertex = vs;
        shader.fragment = fs;
        shader.shaderd = shared;
        shader.name = shader.name.empty() ? shader_cache_id : shader.name;
        
        shader_map_.insert({shader_cache_id, shader});
    }
    
    return shader_cache_id;
}

void GLShaderLibrary::DeleteShader(const std::string& cache_id) {
    if (program_map_.find(cache_id) != program_map_.end()) {
        auto program = program_map_[cache_id];
        delete program;
        program_map_.erase(cache_id);
    } else {
        MLOGE("Delete Shader Failed");
    }
}

ShaderCompileResult GLShaderLibrary::CompileShader(const std::string& cache_id) {
    if (shader_result_.find(cache_id) == shader_result_.end()) {
        if (shader_map_.find(cache_id) != shader_map_.end()) {
            const Shader& shader = shader_map_[cache_id];
            DEBUG_MLOGD("CompileShader cache_id: %s, shader.vertex: %s", cache_id.c_str(), shader.vertex.c_str());
            DEBUG_MLOGD("CompileShader cache_id: %s, shader.fragment: %s", cache_id.c_str(), shader.fragment.c_str());
            GLuint program = this->CreateProgram(shader.vertex, shader.fragment);
            if (program) {
                GLProgram *gl_program = new GLProgram(this->renderer_, program);
                program_map_.insert({ cache_id, gl_program });
                shader_result_.insert({ cache_id, ShaderCompileResult::SUCCESS });
            } else {
                MLOGE("CompoleShader Failed");
                return ShaderCompileResult::FAILED;
            }
        } else {
            MLOGE("CompileShader Failed For Shader not find:%s", cache_id.c_str());
            return ShaderCompileResult::NO_SHADER;
        }
    }
    
    return shader_result_[cache_id];
}

GLuint GLShaderLibrary::CreateProgram(const std::string& vertex_source, const std::string& fragment_source) {
    GLuint program = OpenGLApi::CreateProgram();
    GLuint vertex_shader = CreateShader(GL_VERTEX_SHADER, vertex_source);
    GLuint frag_shader = CreateShader(GL_FRAGMENT_SHADER, fragment_source);
    
    GLint status;
    if (vertex_shader && frag_shader) {
        OpenGLApi::AttachShader(program, vertex_shader);
        OpenGLApi::AttachShader(program, frag_shader);
        OpenGLApi::LinkProgram(program);
        
        OpenGLApi::DeleteShader(vertex_shader);
        OpenGLApi::DeleteShader(frag_shader);
        
        OpenGLApi::GetProgramiv(program, GL_LINK_STATUS, &status);
        if (status != 1) {
            std::string infoLog = OpenGLApi::GetProgramInfoLog(program);
            MLOGE("Failed to link shader %s", infoLog.c_str());
            OpenGLApi::DeleteProgram(program);
            return 0;
        }
        
        return program;
    }
    return 0;
}

GLuint GLShaderLibrary::CreateShader(GLenum shader_type, const std::string& code) {
    GLuint shader = OpenGLApi::CreateShader(shader_type);
    const char* shader_src = code.c_str();
    GLint status;
    
    OpenGLApi::ShaderSource(shader, 1, (const GLchar **)&shader_src);
    OpenGLApi::CompileShader(shader);
    
    OpenGLApi::GetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != 1) {
        std::string infoLog = OpenGLApi::GetShaderInfoLog(shader);
        MLOGE("Failed to compile shader : %s", infoLog.c_str());
        OpenGLApi::DeleteShader(shader);
        return 0;
    }
    
    return shader;
}

GLProgram* GLShaderLibrary::GetProgram(const std::string& shader_cache_id) {
    if (program_map_.find(shader_cache_id) == program_map_.end()) {
        ShaderCompileResult result = this->CompileShader(shader_cache_id);
        if (result == ShaderCompileResult::SUCCESS) {
            return program_map_[shader_cache_id];
        } else {
            return nullptr;
        }
    }
    
    return program_map_[shader_cache_id];
}

std::string GLShaderLibrary::CombineMacros(const std::vector<std::string>& macros) {
    std::stringstream ss;
    for (auto& s: macros) {
        ss << s << "\n";
    }
    return ss.str();
}

std::string GLShaderLibrary::StrHashCode(const std::string& str_1, const std::string& str_2) {
    uint32_t hash = 0;
    int i;
    char chr;
    if (str_1.length() == 0 && str_2.length() == 0) return "0";
    for (i = 0; i < str_1.length(); i++) {
        chr = str_1.at(i);
        hash = ((hash << 5) - hash) + chr;
        hash |= 0;
    }
    for (i = 0; i < str_2.length(); i++) {
        chr = str_2.at(i);
        hash = ((hash << 5) - hash) + chr;
        hash |= 0;
    }
    return std::to_string(hash);
}

void GLShaderLibrary::RemoveUnsharedShader() {
    std::unordered_map<std::string, Shader> temp;
    temp.swap(shader_map_);
    for (auto iter : temp) {
        auto& shader = iter.second;
        if (shader.shaderd) {
            shader_map_.insert({iter.first, iter.second});
        } else {
            std::string name = iter.first;
            {
                auto program_it = program_map_.find(name);
                if (program_it != program_map_.end()) {
                    delete program_it->second;
                    program_map_.erase(program_it);
                }
            }
            shader_result_.erase(name);
        }
    }
}

}
