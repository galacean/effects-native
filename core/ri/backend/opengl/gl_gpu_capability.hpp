//
//  gl_gpu_capability.hpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#ifndef gl_gpu_capability_hpp
#define gl_gpu_capability_hpp

#include <stdio.h>
#include "ri/gl.h"

namespace mn {

class GLGPUCapability {
    
public:
    
    GLGPUCapability();
    
    ~GLGPUCapability();

    int GetMaxFragmentTextures() const;
    
    int GetMaxFragmentUniforms() const;
    
    int GetMaxVertexUniforms() const;
    
    int GetMaxVertexTextures() const;
    
    int GetMaxTextureSize() const;

    bool SupportCompressedTexture() const;
    
    bool SupportHalfFloatTexture() const;
    
    void FrameBufferTexture2D(GLenum target, int index, int tex_target, uint32_t texture) const;
    
    bool DrawBuffers();
    
private:
    
    int max_vertex_uniforms_ = 0;

    int max_vertex_textures_ = 0;
    
    int max_fragment_textures_ = 0;
    
    int max_fragment_uniforms_ = 0;

    bool support_compressed_texture_ = false;

    // texture
    int max_texture_size_ = 0;
    
    // todo: half float内存优化支持
    bool half_float_texture_ = false;
    
};

}

#endif /* gl_gpu_capability_hpp */
