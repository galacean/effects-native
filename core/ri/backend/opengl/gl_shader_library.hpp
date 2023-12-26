//
//  gl_shader_library.hpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#ifndef gl_shader_library_hpp
#define gl_shader_library_hpp

#include <stdio.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "ri/render_base.hpp"
#include "ri/backend/opengl/gl_program.hpp"

namespace mn {

enum ShaderCompileResult {
    NO_SHADER,
    SUCCESS,
    FAILED
};

class GLShaderLibrary {

public:
    
    GLShaderLibrary(GLGPURenderer *renderer);
    
    ~GLShaderLibrary();

    // todo: 10.2.90 macros 和 shader_cache_id 都放到Shader的属性里
//    std::string AddShader(Shader& shader, const std::vector<std::string>& macros, const std::string shader_cached_id);
    
    std::string AddShader(Shader& shader);
    
    void DeleteShader(const std::string& cache_id);
    
    ShaderCompileResult CompileShader(const std::string& cache_id);
    
    // todo: CompileShader Async???
    
    GLProgram* GetProgram(const std::string& shader_cache_id);
    
    void RemoveUnsharedShader();

public:
    
    std::unordered_map<std::string, ShaderCompileResult> shader_result_;
    
private:
#ifdef UNITTEST
public:
#endif
    
    GLGPURenderer *renderer_;
    
    std::string CombineMacros(const std::vector<std::string>& macros);
    
    std::string StrHashCode(const std::string& str_1, const std::string& str_2);
    
    GLuint CreateProgram(const std::string& vertex_source, const std::string& fragment_source);
    
    GLuint CreateShader(GLenum shader_type, const std::string& code);
        
    std::unordered_map<std::string, Shader> shader_map_;
    
    std::unordered_map<std::string, GLProgram *> program_map_;
    
};

}

#endif /* gl_shader_library_hpp */
