//
//  gl_state.hpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#ifndef gl_state_hpp
#define gl_state_hpp

#include <stdio.h>
#include <unordered_map>
#include "ri/backend/opengl/gl_api.hpp"

namespace mn {

class GLState {
    
public:
    
    GLState();
    
    ~GLState();
    
    void Reset();
    
    void Enable(GLenum capability);
    
    void Disable(GLenum capability);
    
    void BindFrameBuffer(GLenum target, GLuint frameBuffer);
    
    void BindRenderBuffer(GLenum target, GLuint renderBuffer);
    
    void UseProgram(GLuint program) const;
    
    void DepthFunc(GLenum func);
    
    void DepthMask(GLboolean flag);
    
    void Clear(GLbitfield mask);
    
    void DepthRangef(GLfloat nearVal, GLfloat farVal);
    
    void StencilFunc(GLenum func, GLint ref, GLuint mask);
    
    void StencilFuncSeparate(GLuint face, GLenum func, GLint ref, GLuint mask);
    
    void StencilMask(GLuint mask);
    
    void StencilMaskSeparate(GLuint face, GLuint mask);
    
    void StencilOp(GLenum sfail, GLenum dpfail, GLenum dppass);
    
    void StencilOpSeparate(GLuint face, GLenum sfail, GLenum dpfail, GLenum dppass);
    
    void ColorMask(GLboolean red,
                   GLboolean green,
                   GLboolean blue,
                   GLboolean alpha);
    
    void CullFace(GLenum mode);
    
    void FrontFace(GLenum mode);
    
    void BlendColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
    
    void BlendFuncSeparate(GLenum srcRGB,
                           GLenum dstRGB,
                           GLenum srcAlpha,
                           GLenum dstAlpha);
    
    void BlendEquationSeparate(GLenum modeRGB,
                               GLenum modeAlpha);
    
    void PixelStorei(GLenum pname, GLint param);
    
    void ViewPort(int32_t x, int32_t y, int32_t w, int32_t h);
    
    void ActiveTexture(GLenum texture);
    
    void BindTexture(GLenum target, GLuint texture);
    
    void GetActiveTextureUnit();
    
private:
        
    std::unordered_map<GLenum, bool> dict_;
    
    GLuint current_framebuffer_;
    
    GLuint current_texture_binding_;
    
    GLuint current_render_buffer_;
    
};

}

#endif /* gl_state_hpp */
