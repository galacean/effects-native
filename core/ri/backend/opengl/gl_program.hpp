//
//  gl_program.hpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#ifndef gl_program_hpp
#define gl_program_hpp

#include <stdio.h>
#include <string>
#include <unordered_map>
#include "ri/gl.h"
#include "ri/backend/opengl/gl_geometry.hpp"
#include "ri/render/semantic_value.hpp"
#include "ri/backend/opengl/gl_texture.hpp"

namespace mn {

// program
struct ProgramAttributeInfo {
    
    ProgramAttributeInfo(const std::string& n, GLint loc, GLenum t, GLsizei s) :
        name(n), location(loc), type(t), size(s) {
            
    }
    
    std::string name;
    
    // index
    GLint location;
    
    // data type;
    GLenum type;
    
    // size of the uniform variable, uPos[8]  size = 8
    GLsizei size;
    
};

struct ProgramUniformInfo {
    
    ProgramUniformInfo(const std::string& n, GLint loc, GLsizei s, GLenum t) :
        name(n), location(loc), type(t), size(s), texture_index(0) {
        
    }
    
    GLint location;
    
    std::string name;
    
    GLsizei size;
    
    GLenum type;
    
    int texture_index;

};

class GLGPURenderer;

class GLProgram {
    
public:
    
    GLProgram(GLGPURenderer *renderer, GLuint program);
    
    ~GLProgram();
    
    void Init();
    
    void SetupUniforms(const RenderState& state);
    
    void SetupAttributes(GLGeometry *geometry);
    
    void Bind();
    
    void UnBind();

private:
    
    void InitUniforms();
    
    void InitAttributes();
    
    void SetGLUniformValue(const std::string& name, const ProgramUniformInfo& uniform_info, std::shared_ptr<UniformValue> uniform_value);
    
    std::unordered_map<std::string, ProgramAttributeInfo> attribute_map_;
    
    std::unordered_map<std::string, ProgramUniformInfo> uniform_map_;
    
    GLGPURenderer *renderer_ = nullptr;
    
    GLTexture *empty_texture_ = nullptr;
    
    GLuint program_ = 0;
};

}

#endif /* gl_program_hpp */
