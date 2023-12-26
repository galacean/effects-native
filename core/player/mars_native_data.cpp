#include "include/mars_native_player.h"
#include "sceneData/mars_data_base.h"
#include "sceneData/scene_data_decoder.h"
#include "util/util.hpp"
#include "util/log_util.hpp"
#include "util/image_loader.h"

namespace mn {

class MarsNativeDataImpl : public MarsNativeData {
public:
    MarsNativeDataImpl(uint8_t* data, MarsNativeImageDecoder* image_decoder) {
        buffer_ = new MBuffer(data);
        int version = buffer_->ReadInt();
        if (version != 1) {
            MLOGE("MarsNativeData invalid version %d", version);
            return;
        }
        width_ = buffer_->ReadInt();
        height_ = buffer_->ReadInt();
        if (width_ <= 0 && height_ <= 0) {
            MLOGE("MarsNativeData invalid size %d %d", width_, height_);
            return;
        }
        
        int plugin_count = buffer_->ReadInt();
        for (int i = 0; i < plugin_count; i++) {
            const char* plugin = buffer_->ReadString();
            plugin_list_.push_back(plugin);
        }
        
        int image_list_length = buffer_->ReadInt();
        for (int i = 0; i < image_list_length; i++) {
            const char* url = buffer_->ReadString();
            int image_buffer_length = buffer_->ReadInt();
            uint8_t* image_buffer = buffer_->ReadUint8Array(image_buffer_length);
            MarsNativeImageData* image_data = nullptr;
            // 非压缩纹理需要decode
            if (!ImageLoader::IsKtxImage(image_buffer, image_buffer_length)) {
                int width = 0;
                int height = 0;
                bool premultiply_alpha = false;
                if (image_decoder) {
                    image_buffer = image_decoder->Decode(width, height, premultiply_alpha, image_buffer, image_buffer_length);
                } else {
                    ImageRawData* raw_data = new ImageRawData(image_buffer, image_buffer_length);
                    raw_data->need_free = false;
                    image_buffer = nullptr;
                    ImageData* decode_data = ImageLoader::ParseImageData(raw_data);
                    if (decode_data) {
                        image_buffer = decode_data->data;
                        width = decode_data->width;
                        height = decode_data->height;
                        premultiply_alpha = decode_data->premultiply_alpha;
                        decode_data->data = nullptr;
                        delete decode_data;
                    }
                }
                if (!image_buffer) {
                    MLOGE("MarsNativeData image decode fail");
                    return;
                }
                image_data = new MarsNativeImageData(url, image_buffer, true);
                image_data->width = width;
                image_data->height = height;
                image_data->premultiply_alpha = premultiply_alpha;
            } else {
                // 压缩纹理
                image_data = new MarsNativeImageData(url, image_buffer, false);
                image_data->buffer_length = image_buffer_length;
            }
            image_list_.push_back(image_data);
        }

        mars_data_buffer_length_ = buffer_->ReadInt();
        if (mars_data_buffer_length_ <= 0) {
            MLOGE("MarsNativeData invalid buffer length %d", mars_data_buffer_length_);
            return;
        }
        mars_data_buffer_ = buffer_->ReadUint8Array(mars_data_buffer_length_);
        MLOGD("MarsNativeData image count %d data buffer length %d", image_list_length, mars_data_buffer_length_);
        
        valid_ = true;
    }

    virtual ~MarsNativeDataImpl() {
        MN_SAFE_DELETE(buffer_);
        if (!image_list_.empty()) {
            for (int i = 0; i < image_list_.size(); i++) {
                delete image_list_[i];
            }
            image_list_.clear();
        }
    }

    int GetWidth() const override {
        return width_;
    }

    int GetHeight() const override {
        return height_;
    }
    
    void* GetSceneData() const override {
        if (mars_data_buffer_length_ == 0 || !mars_data_buffer_) {
            MLOGE("MarsNativeData invalid buffer %d %p", mars_data_buffer_length_, mars_data_buffer_);
            return nullptr;
        }
        void* ret = malloc(mars_data_buffer_length_);
        if (!ret) {
            MLOGE("MarsNativeData malloc fail %d", mars_data_buffer_length_);
            return nullptr;
        }
        memcpy(ret, mars_data_buffer_, mars_data_buffer_length_);
        return ret;
    }

    int GetImageDataCount() const override {
        return image_list_.size();
    }
    
    MarsNativeImageData* GetImageDataAt(int idx) const override {
        if (idx < 0 || idx >= image_list_.size()) {
            return nullptr;
        }
        return image_list_[idx];
    }
    
    int GetPluginCount() const override {
        return plugin_list_.size();
    }
    
    std::string GetPluginAt(int idx) const override {
        if (idx < 0 || idx >= plugin_list_.size()) {
            return nullptr;
        }
        return plugin_list_[idx];
    }

public:
    MBuffer* buffer_ = nullptr;
    std::vector<MarsNativeImageData*> image_list_;
    
    std::vector<std::string> plugin_list_;

    int width_ = 0;
    int height_ = 0;
    uint8_t* mars_data_buffer_ = nullptr;
    int mars_data_buffer_length_ = 0;

    bool valid_ = false;
};

MarsNativeData* MarsNativeData::CreateNew(void* buffer, int length, bool need_free, MarsNativeImageDecoder* image_decoder) {
    if (!buffer) {
        return nullptr;
    }
    uint8_t* data;
    if (need_free) {
        data = (uint8_t*) buffer;
    } else {
        data = (uint8_t*) malloc(length);
        memcpy(data, buffer, length);
    }
    MarsNativeDataImpl* ret = new MarsNativeDataImpl(data, image_decoder);
    if (!ret->valid_) {
        delete ret;
        return nullptr;
    }
    return ret;
}

}
