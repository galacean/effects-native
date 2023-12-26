//
//  mars_geometry.hpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#ifndef mars_geometry_hpp
#define mars_geometry_hpp

#include <stdio.h>
#include "ri/backend/opengl/gl_geometry.hpp"
#include "ri/render_base.hpp"
#include "util/log_util.hpp"

namespace mn {

struct DirtyFlag {
    
    DirtyFlag() {
        dirty = false;
        discard = false;
        start = 0;
        end = 0;
    }
    
    DirtyFlag(bool d, bool dis, size_t s, size_t e)
            : dirty(d), discard(dis), start(s), end(e) {
    }
    
    bool dirty;
    
    bool discard;
    
    size_t start;
    
    size_t end;
};

// Attribute Data的实际保存地;
struct GPUBufferOption {
    
    GPUBufferOption() : data(nullptr), type(BufferDataType::FLOAT), byte_length(0) { }
    
    GPUBufferOption(BufferDataType t) : data(nullptr), type(t), byte_length(0) {}
    
    ~GPUBufferOption() {
        if (data) {
            free(data);
        }
    }
    
    void Reset() {
        if (data) {
            free(data);
            data = nullptr;
        }
        byte_length = 0;
    }
    
    void SetData(uint8_t *d, size_t byte_len) {
        uint8_t* old_data = data;
        uint8_t* new_data = (uint8_t *)malloc(byte_len);
        memset(new_data, 0, byte_len);
        if (old_data) {
            free(old_data);
        }
        
        memcpy(new_data, d, byte_len);
        data = new_data;
        byte_length = byte_len;
    }
    
    void SetOffsetData(uint8_t *d, size_t offset, size_t byte_len) {
        if (byte_length >= offset + byte_len) {
            memcpy(data + offset, d, byte_len);
        } else {
            MLOGE("GPUBufferOption SetOffsetData Failed");
        }
    }
    
    void SetData(float value, size_t index) {
        float* data_view = (float*)data;
        if (index * sizeof(float) < byte_length) {
            data_view[index] = value;
        }
    }
    
    void EnlargeBuffer(size_t enlarge_length) {
        if (byte_length < enlarge_length) {
            uint8_t* new_buffer = (uint8_t*) malloc(enlarge_length);
            memset(new_buffer, 0, enlarge_length);
            if (data) {
                memcpy(new_buffer, data, byte_length);
                free(data);
                data = nullptr;
            }
            data = new_buffer;
            byte_length = enlarge_length;
        } else {
            MLOGE("enlargeLenght short the byteLength");
        }
    }
    
    uint8_t *data;
    
    BufferDataType type;
    
    size_t byte_length;
    
    /**
     * 一个顶点属性包含元素数量，如position一般是3个float组成，itemSize即为3
     */
    int item_size = 0;
    
};

class MarsRenderer;

class MarsGeometry {
    
public:
    
    MarsGeometry(MarsRenderer* renderer, int draw_count, int draw_start, GLenum mode = GL_TRIANGLES);
    
    MarsGeometry(MarsRenderer* renderer, GLenum mode = GL_TRIANGLES);
    
    ~MarsGeometry();
    
    void AssignRenderer(MarsRenderer *renderer);
    
    void SetAttributeInfo(const std::string& name, const AttributeWithData& attribute_data);
    
    void SetIndexInfo(std::shared_ptr<GPUBufferOption> index_buffer);
    
//    void SetAttributeBuffer(const std::string& name, std::shared_ptr<GLGPUBuffer> buffer);
    
    void SetAttributeData(const std::string& name, uint8_t *data, size_t byte_length);
    
    // todo:
    // void GetAttributeData(const std::string& name) {}
    
    void SetAttributeSubData(const std::string& name, uint8_t *data, size_t byte_offset, size_t byte_length);
    
    void SetIndexData(uint8_t *data, size_t byte_length);
    
    void SetIndexSubData(uint8_t *data, size_t offset, size_t byte_length);
    
    void NeedUpdateAttributeData(const std::string& name);
        
    void NeedUpdateIndexData();
    
    uint32_t GetAttributeStride(const std::string& name);
    
    uint32_t GetAttributeDataLength(const std::string& name);
    
    std::shared_ptr<GPUBufferOption> GetAttributeBufferOption(const std::string& name) {
        if (buffer_datas_.find(name) != buffer_datas_.end()) {
            return buffer_datas_[name];
        }
        return nullptr;
    }
    
    std::shared_ptr<GPUBufferOption>& GetIndexBufferOption() {
        return index_buffer_data_;
    }
                
    uint32_t GetDrawCount() const {
        return draw_count_;
    }
    
    void SetDrawCount(uint32_t n) {
        draw_count_ = n;
        if (inner_geometry_) {
            inner_geometry_->SetDrawCount(n);
        }
    }
    
    bool Flush();
    
    void Draw();
    
    void ResetGeometryData();
        
    GLGeometry *InnerGeometry() {
        return inner_geometry_;
    }
    
    std::vector<std::string> GetAttributeNames() {
        std::vector<std::string> keys;
        for (auto iter : attribute_datas_) {
            keys.push_back(iter.first);
        }
        
        return keys;
    }

private:
        
    std::unordered_map<std::string, DirtyFlag> dirty_flags_;
    
    std::vector<std::string> attribute_names_;
    
    std::unordered_map<std::string, std::shared_ptr<GPUBufferOption>> buffer_datas_;
    
    MarsRenderer *renderer_ = nullptr;
    
    std::unordered_map<std::string, AttributeBase> attribute_datas_;
    
    GLGeometry* inner_geometry_ = nullptr;
    
    std::shared_ptr<GPUBufferOption> index_buffer_data_;
    
    uint32_t draw_count_ = 0;
    
    uint32_t draw_start_ = 0;
    
    GLenum mode_;
        
};

}

#endif /* mars_geometry_hpp */
