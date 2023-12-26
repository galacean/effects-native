//
//  render_base.cpp
//
//  Created by Zongming Liu on 2022/4/7.
//

#include "render_base.hpp"
#include "ri/utils.hpp"
#include "util/log_util.hpp"

namespace mn {

UniformValue::UniformValue(UniformDataType type) : data(nullptr), data_type(type), is_semantic(false) {
    byte_length = Utils::ByteLengthOfUniformData(type);
    data = (uint8_t *)malloc(byte_length);
    memset(data, 0, byte_length);
}

UniformValue::UniformValue(UniformDataType type, bool semantic, const std::string& name) : data(nullptr), data_type(type), is_semantic(semantic), semantic_name(name) {
    byte_length = Utils::ByteLengthOfUniformData(type);
    data = (uint8_t *)malloc(byte_length);
    memset(data, 0, byte_length);
}

UniformValue::UniformValue(UniformDataType type, size_t size) : data(nullptr), data_type(type), is_semantic(false) {
    byte_length = Utils::ByteLengthOfUniformData(type) * size;
    data = (uint8_t *)malloc(byte_length);
    memset(data, 0, byte_length);
}

UniformValue::~UniformValue() {
    if (data) {
        free(data);
    }
}


void UniformValue::SetData(const mn::Vec2& vec2, size_t index) {
    float *dst = reinterpret_cast<float *>(data);
    for (size_t i=0; i<vec2.Length(); i++) {
        dst[index + i] = vec2.m[i];
    }
}

// todo: index这个参数设计好奇怪？？？
void UniformValue::SetData(const mn::Vec3& vec3, size_t index) {
    float *dst = reinterpret_cast<float *>(data);
    for (size_t i=0; i<vec3.Length(); i++) {
        dst[index + i] = vec3.m[i];
    }
}

void UniformValue::SetData(const mn::Vec4& vec4, size_t index) {
    float *dst = reinterpret_cast<float *>(data);
    for (size_t i=0; i<vec4.Length(); i++) {
        dst[index + i] = vec4.m[i];
    }
}

void UniformValue::SetData(const mn::Mat3& mat3) {
    if (data_type == UniformDataType::FLOAT_MAT3) {
        size_t len = mat3.Length();
        float *dst = reinterpret_cast<float *>(data);
        for (size_t i=0; i<len; i++) {
            dst[i] = mat3.m[i];
        }
    } else {
        MLOGE("set uniformValue not match mat3 data type");
    }
}

void UniformValue::SetData(const mn::Mat4& mat4) {
    if (data_type == UniformDataType::FLOAT_MAT4) {
        size_t len = mat4.Length();
        float *dst = reinterpret_cast<float *>(data);
        for (size_t i=0; i<len; i++) {
            dst[i] = mat4.m[i];
        }
    } else {
        MLOGE("set uniformValue not match mat4 data type");
    }
}

void UniformValue::SetData(uint8_t* d, size_t byte_len) {
    if (byte_length == byte_len) {
        // todo: 是否安全
        memcpy(data, d, byte_len);
    }
}

void UniformValue::SetData(float *src, size_t index, size_t length) {
    float *dst = reinterpret_cast<float *>(data);
    for (int i=0; i<length; i++) {
        dst[index + i] = src[i];
    }
}

void UniformValue::SetData(int value) {
    int *dst = reinterpret_cast<int *>(data);
    dst[0] = value;
}
    
void UniformValue::SetData(float src, size_t index) {
    float *dst = reinterpret_cast<float *>(data);
    dst[index] = src;
}

void UniformValue::ResetData(void* data) {
    if (this->data) {
        free(this->data);
        this->data = nullptr;
    }
    this->data = (uint8_t*) data;
}

void UniformValue::SetTexture(std::shared_ptr<MarsTexture> tex) {
    texture = tex;
}

}

