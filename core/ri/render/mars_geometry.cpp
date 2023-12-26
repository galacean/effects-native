//
//  mars_geometry.cpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#include <algorithm>
#include "mars_geometry.hpp"
#include "util/log_util.hpp"
#include "ri/utils.hpp"
#include "ri/render/mars_renderer.hpp"

namespace mn {

const std::string INDEX_STRING = "index";

MarsGeometry::MarsGeometry(MarsRenderer* renderer, int draw_count, int draw_start, GLenum mode) : draw_count_(draw_count), draw_start_(draw_start), mode_(mode), renderer_(nullptr), inner_geometry_(nullptr) {
    if (renderer) {
        this->AssignRenderer(renderer);
    }
}

MarsGeometry::MarsGeometry(MarsRenderer* renderer, GLenum mode): renderer_(nullptr), draw_count_(0), draw_start_(0), mode_(mode) {
    if (renderer) {
        this->AssignRenderer(renderer);
    }
}

MarsGeometry::~MarsGeometry() {
//    MLOGD("MarsGeometry Destruct");
    if (inner_geometry_) {
        delete inner_geometry_;
    }
}

// todo: 10.2.90 assignRender重构;
void MarsGeometry::AssignRenderer(MarsRenderer *renderer) {
    if (!renderer_) {
        renderer_ = renderer;
        inner_geometry_ = new GLGeometry(renderer_->InnerRenderer(), mode_);
        inner_geometry_->SetDrawStart(draw_start_);
        inner_geometry_->SetDrawCount(draw_count_);
    }
}

void MarsGeometry::SetAttributeInfo(const std::string& name, const AttributeWithData& attribute_data) {
    if (attribute_data.attribute_base.data_source.empty()) {
        auto buffer_data = std::make_shared<GPUBufferOption>();
        if (attribute_data.data) {
            buffer_data->SetData(attribute_data.data, attribute_data.byte_length);
        }
        
        buffer_datas_.insert({ name, buffer_data });
        DirtyFlag flag(true, true, 0, 0);
        dirty_flags_.insert({ name, flag });
        
        // todo: 理下这里的逻辑
        if (attribute_data.attribute_base.data_source.empty()) {
//            attribute_data.attribute_base.data_source = name;
            auto gpu_buffer = std::make_shared<GLGPUBuffer>(renderer_->InnerRenderer() ,GL_ARRAY_BUFFER, GL_FLOAT, GL_STATIC_DRAW);
            this->inner_geometry_->SetAttributeBuffer(name, gpu_buffer);
        } else {
            this->inner_geometry_->SetAttributeBuffer(name, attribute_data.attribute_base.data_source);
        }
    }
    
    attribute_datas_.insert({ name, attribute_data.attribute_base });
    
    auto& attribute_base = attribute_datas_[name];
    if (attribute_base.data_source.empty()) {
        attribute_base.data_source = name;
    }
    
    attribute_names_.push_back(name);
    this->inner_geometry_->SetAttributeInfo(name, attribute_base);
}

void MarsGeometry::SetIndexInfo(std::shared_ptr<GPUBufferOption> index_buffer) {
    inner_geometry_->CreateIndexBuffer(Utils::ConvertToGLType(index_buffer->type));
    index_buffer_data_ = index_buffer;
    DirtyFlag flag(true, true, 0, 0);
    dirty_flags_.insert({ INDEX_STRING, flag });
}

void MarsGeometry::SetAttributeData(const std::string& name, uint8_t *data, size_t byte_length) {
    if (attribute_datas_.find(name) != attribute_datas_.end()) {
        auto& attribute_data = attribute_datas_[name];
        std::string data_source = attribute_data.data_source;
        
        if (buffer_datas_.find(data_source) != buffer_datas_.end()
            && dirty_flags_.find(data_source) != dirty_flags_.end()) {
            std::shared_ptr<GPUBufferOption> buffer_option = std::make_shared<GPUBufferOption>(BufferDataType::FLOAT);
            
            buffer_option->SetData(data, byte_length);
            buffer_datas_[data_source] = buffer_option;
            
            DirtyFlag& flag = dirty_flags_[data_source];
            flag.discard = true;
            flag.dirty = true;
//            MLOGD("Geometry::SetAttributeData: name: %s, size:%d", name.data(), byte_length);
        }
    }
}

// 外部用float[] 来处理数据；
void MarsGeometry::SetAttributeSubData(const std::string& name, uint8_t *data, size_t byte_offset, size_t byte_length) {
    if (buffer_datas_.find(name) != buffer_datas_.end()) {
        auto buffer_data = buffer_datas_[name];
        if (buffer_data->data) {
            size_t start = byte_offset;
            size_t length = start + byte_length;
            
            DirtyFlag& dirty_flag = dirty_flags_[name];
            dirty_flag.discard = false;
            if (buffer_data->byte_length < length) {
                MLOGD("Attribute SubData length over range: %s", name.data());
                uint8_t *old_data = buffer_data->data;
                uint8_t *new_data = (uint8_t *) malloc(length);
                memset(new_data, 0, length);
                
                if (old_data) {
                    memcpy(new_data, old_data, length);
                    free(old_data);
                    buffer_data->data = nullptr;
                }
                
                buffer_data->data = new_data;
                dirty_flag.discard = true;
                MLOGD("Geometry::SetAttribute SubData name: %s", name.data());
            } else if (!dirty_flag.discard) {
                dirty_flag.start = std::min(dirty_flag.start, start);
                dirty_flag.end = std::max(dirty_flag.end, length - 1);
//                MLOGD("Geometry::SetAttribute name: %s subdata start:%d, end %d", name.data(), dirty_flag.start, dirty_flag.end);
            }
            
            memcpy((buffer_data->data + byte_offset), data, byte_length);
            dirty_flag.dirty = true;
        }
    }
}

void MarsGeometry::SetIndexData(uint8_t *data, size_t byte_length) {
    if (index_buffer_data_) {
        index_buffer_data_->SetData(data, byte_length);
//        MLOGD("Geometry::SetIndexData length: %d", byte_length);
        if (dirty_flags_.find(INDEX_STRING) != dirty_flags_.end()) {
            DirtyFlag& flag = dirty_flags_[INDEX_STRING];
            flag.discard = true;
            flag.dirty = true;
        } else {
            MLOGE("Index Buffer Dirty Flag is NULL");
        }
    } else {
        MLOGE("Not Init Geometry Index Data");
    }
}

void MarsGeometry::NeedUpdateAttributeData(const std::string& name) {
    if (buffer_datas_.find(name) != buffer_datas_.end()) {
        if (dirty_flags_.find(name) != dirty_flags_.end()) {
            DirtyFlag& dirty_flag = dirty_flags_[name];
            dirty_flag.discard = true;
            dirty_flag.dirty = true;
        } else {
            MLOGE("Geometry::UpdateAttributeData %s failed dirty flag not exits;", name.data());
        }
    } else {
        MLOGE("Geometry::UpdateAttributeData %s failed buffer_datas not exits", name.data());
    }
}

void MarsGeometry::NeedUpdateIndexData() {
    if (dirty_flags_.find(INDEX_STRING) != dirty_flags_.end()) {
        DirtyFlag& flag = dirty_flags_[INDEX_STRING];
        flag.discard = true;
        flag.dirty = true;
    } else {
        MLOGE("Index Buffer Dirty Flag is NULL");
    }
}

void MarsGeometry::SetIndexSubData(uint8_t *data, size_t offset, size_t byte_length) {
    if (index_buffer_data_) {
        DirtyFlag& index_flag = dirty_flags_[INDEX_STRING];
        index_flag.discard = false;
        
        size_t start = offset;
        size_t length = offset + byte_length;
        
        if (index_buffer_data_->byte_length < length) {
            MLOGD("Geometry::Index SubData length over range");
            uint8_t *old_data = index_buffer_data_->data;
            uint8_t *new_data = (uint8_t *) malloc(length);
            memset(new_data, 0, length);
            if (old_data) {
                memcpy(new_data, old_data, length);
                free(old_data);
                index_buffer_data_->data = nullptr;
            }
            index_buffer_data_->data = new_data;
            index_flag.discard = true;
            MLOGD("Geometry::SetIndexSubData length:%d", byte_length);
        } else if (!index_flag.discard) {
            index_flag.start = std::min(index_flag.start, start);
            index_flag.end = std::max(index_flag.end, length - 1);
//            MLOGD("Geometry::SetIndexSubData subdata start:%d, end %d", index_flag.start, index_flag.end);
        }
        memcpy(index_buffer_data_->data + offset, data, byte_length);
        index_flag.dirty = true;
    } else {
        MLOGE("Not Init Geometry Index SubData");
    }
}

uint32_t MarsGeometry::GetAttributeStride(const std::string& name) {
    if (attribute_datas_.find(name) != attribute_datas_.end()) {
        auto& attribute_data = attribute_datas_[name];
        uint32_t stride = attribute_data.stride;
        return stride;
    } else {
        MLOGE("Attribute not exits %s", name.data());
        return 0;
    }
}

uint32_t MarsGeometry::GetAttributeDataLength(const std::string& name) {
    return 0.0;
}

void MarsGeometry::Draw() {
    inner_geometry_->Draw();
}

bool MarsGeometry::Flush() {
    for (auto& iter : dirty_flags_) {
        std::string name = iter.first;
        DirtyFlag& dirty_flag = iter.second;
        
        if (dirty_flag.dirty) {
            std::shared_ptr<GPUBufferOption> buffer_data;
            std::shared_ptr<GLGPUBuffer> gpu_buffer;
            if (name == INDEX_STRING) {
                buffer_data = this->index_buffer_data_;
                gpu_buffer = this->inner_geometry_->GetIndexBuffer();
            } else {
                buffer_data = buffer_datas_[name];
                gpu_buffer = inner_geometry_->GetAttributeBuffer(name);
            }
            
            if (buffer_data && gpu_buffer) {
                if (dirty_flag.discard) {
                    DEBUG_DATA("Geometry::Flush Buffer " + name, buffer_data->data, buffer_data->byte_length, Utils::SizeOfDataType(buffer_data->type));
                    gpu_buffer->BufferData(buffer_data->byte_length, buffer_data->data);
                } else {
                    size_t offset = dirty_flag.start;
                    uint32_t length = dirty_flag.end - dirty_flag.start + 1;
                    DEBUG_DATA("Geometry::Flush SubBuffer " + name, buffer_data->data + offset, length, Utils::SizeOfDataType(buffer_data->type));
                    gpu_buffer->BufferSubData(offset, length, buffer_data->data + offset);
                }
                dirty_flag.dirty = false;
            } else {
                MLOGE("Geomtry::Flush Failed for %s buffer Data || gpu buffer is null", name.data());
            }
        }
    }
    
    return true;
}

void MarsGeometry::ResetGeometryData() {
    this->draw_count_ = 0;
    this->draw_start_ = 0;
    
    for (auto iter : buffer_datas_) {
        iter.second->Reset();
    }
    index_buffer_data_->Reset();
}

}
