#include "mars_data_base.h"
#include <sys/stat.h>
#include "util/log_util.hpp"

namespace mn {

MBuffer::MBuffer(uint8_t* buffer) {
    buffer_ = buffer;
    int_buffer_ = (int*)buffer;
    float_buffer_ = (float*)buffer;
    uint32_buffer_ = (uint32_t*)buffer;
}

MBuffer::~MBuffer() {
    free(buffer_);
}

int MBuffer::ReadInt() {
    return int_buffer_[idx_++];
}

uint32_t MBuffer::ReadUint32() {
    return uint32_buffer_[idx_++];
}

float MBuffer::ReadFloat() {
    return float_buffer_[idx_++];
}

const char* MBuffer::ReadString() {
    const char* str = (const char*)(buffer_ + idx_ * 4);
    idx_ += strlen(str) / 4 + 1;
    return str;
}

int* MBuffer::ReadIntArray(int length) {
    int* arr = (int_buffer_ + idx_);
    idx_ += length;
    return arr;
}

float* MBuffer::ReadFloatArray(int length) {
    float* arr = (float_buffer_ + idx_);
    idx_ += length;
    return arr;
}

uint8_t* MBuffer::ReadUint8Array(int byte_len) {
    uint8_t* ret = (uint8_t*) (float_buffer_ + idx_);
    idx_ += (byte_len + 3) / 4;
    return ret;
}

std::shared_ptr<MValue> MBuffer::ReadValue() {
    std::shared_ptr<MValue> value = std::make_shared<MValue>();
    value->type_ = ReadString();
    value->length_ = ReadInt();
    value->data_ = ReadFloatArray(value->length_);
    if (value->type_.compare("bezier") == 0) {
        value->length2_ = ReadInt();
        value->data2_ = ReadFloatArray(value->length2_);
    }
    return value;
}

void* MBuffer::ReadTypedArray(uint32_t& out_len, uint32_t& out_type) {
    out_type = ReadUint32();
    out_len = ReadUint32();
    return ReadUint8Array(out_len);
}

uint8_t* MImageData::GetKtxImage() {
    FILE* fp = fopen(file_path_.c_str(), "rb");
    if (fp == NULL) {
        MLOGE("GetKtxImage fp is null [%s]", file_path_.c_str());
        return nullptr;
    }
    auto descriptor = fileno(fp);
    struct stat stat_buf;
    if (fstat(descriptor, &stat_buf) == -1) {
        fclose(fp);
        MLOGE("GetKtxImage stat invalid");
        return nullptr;
    }
    size_t buffer_length = static_cast<size_t>(stat_buf.st_size);
    uint8_t* buffer = (uint8_t*)malloc(buffer_length);

    size_t read_size = 0;
    read_size = fread(buffer, sizeof(unsigned char), buffer_length, fp);

    if (read_size != buffer_length) {
        free(buffer);
        fclose(fp);
        MLOGE("GetKtxImage size error %ld %ld", read_size, buffer_length);
        return nullptr;
    }

    fclose(fp);
    return buffer;
}

}
