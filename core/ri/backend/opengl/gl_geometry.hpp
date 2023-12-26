//
//  gl_geometry.hpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#ifndef gl_geometry_hpp
#define gl_geometry_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <unordered_map>

#include "ri/backend/opengl/gl_gpu_buffer.hpp"
#include "ri/render_base.hpp"

#include "ri/backend/opengl/gl_api.hpp"

// todo: 1. vao;
namespace mn {

class GLGPURenderer;

class GLGeometry {
    
public:
    
    GLGeometry(GLGPURenderer *renderer_, GLenum mode = GL_TRIANGLES);
        
    ~GLGeometry();
        
    std::shared_ptr<GLGPUBuffer> GetAttributeBuffer(const std::string& name);
    
    // todo:需要优化下，这里不能直接返回引用;
    AttributeBase* GetAttributeBase(const std::string& name);
    
    std::shared_ptr<GLGPUBuffer> GetIndexBuffer();
    
    void SetAttributeBuffer(const std::string& name, std::shared_ptr<GLGPUBuffer> buffer);
    
    void SetAttributeBuffer(const std::string& name, const std::string& source);
    
    void SetAttributeInfo(const std::string& name, const AttributeBase& attribute);
    
    void CreateIndexBuffer(GLenum buffer_type);
    
    void SetDrawCount(uint32_t count);
    
    void SetDrawStart(uint32_t start);
    
    void Draw();
    
private:
    
    GLGPURenderer *renderer_ = nullptr;
    
    // drawElement mode;
    GLenum mode_;;
    
    uint32_t draw_count_ = 0;
    
    uint32_t draw_start_ = 0;
    
    // GLProgram中绑定的属性关系;
    std::unordered_map<std::string, AttributeBase> attribute_info_map_;
    
    std::unordered_map<std::string, std::shared_ptr<GLGPUBuffer>> buffer_map_;
    
    std::shared_ptr<GLGPUBuffer> index_buffer_;
        
};

}

#endif /* gl_geometry_hpp */
