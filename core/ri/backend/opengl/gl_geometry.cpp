//
//  gl_geometry.cpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#include "gl_geometry.hpp"
#include "util/log_util.hpp"

namespace mn {

GLGeometry::GLGeometry(GLGPURenderer *renderer, GLenum mode) : renderer_(renderer), mode_(mode) {
    
}

GLGeometry::~GLGeometry() {
//    MLOGD("GLGeometry Destroyed");
}
    
std::shared_ptr<GLGPUBuffer> GLGeometry::GetAttributeBuffer(const std::string& name) {
    if (buffer_map_.find(name) != buffer_map_.end()) {
        return buffer_map_[name];
    }
    
    return nullptr;
}

AttributeBase* GLGeometry::GetAttributeBase(const std::string& name) {
    if (attribute_info_map_.find(name) != attribute_info_map_.end()) {
        AttributeBase* attribute_base = &(attribute_info_map_[name]);
        return attribute_base;
    }
    
    return nullptr;
}

std::shared_ptr<GLGPUBuffer> GLGeometry::GetIndexBuffer() {
    return index_buffer_;
}

void GLGeometry::SetAttributeBuffer(const std::string& name, std::shared_ptr<GLGPUBuffer> buffer) {
    buffer_map_[name] = buffer;
}

// 共享source的attributeBuffer;
void GLGeometry::SetAttributeBuffer(const std::string& name, const std::string& source) {
    buffer_map_[name] = buffer_map_[source];
}

void GLGeometry::SetAttributeInfo(const std::string& name, const AttributeBase& attribute) {
    attribute_info_map_[name] = attribute;
}

// 为了保持和web创建buffer顺序一致
void GLGeometry::CreateIndexBuffer(GLenum buffer_type) {
    index_buffer_ = std::make_shared<GLGPUBuffer>(renderer_, GL_ELEMENT_ARRAY_BUFFER, buffer_type, GL_STATIC_DRAW);
}

void GLGeometry::SetDrawCount(uint32_t count) {
    draw_count_ = count;
}

void GLGeometry::SetDrawStart(uint32_t start) {
    draw_start_ = start;
}

void GLGeometry::Draw() {
    if (index_buffer_) {
        size_t index_buffer_elements = index_buffer_->ElementCount();
        if (index_buffer_elements >= (draw_start_ + draw_count_)) {
            index_buffer_->Bind();
            OpenGLApi::DrawElements(mode_, draw_count_, index_buffer_->Type(), (const void *) draw_start_);
        } else {
            // todo: 10.2.90 添加数据埋点;
            MLOGE("GLGeometry Draw Failed outof index elements %zu, drawCount %zu, drawStart %zu", index_buffer_elements, draw_count_, draw_start_);
        }
    } else {
        MLOGD("GLGeometry DrawArray used");
        OpenGLApi::DrawArrays(mode_, draw_start_, draw_count_);
    }
}

}
