//
//  scene_data_decoder_new.cpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/6/3.
//

#include "scene_data_decoder_new.hpp"
#include <sys/stat.h>
#include "decoder/composition_decoder.hpp"
#include "include/mars_native_player.h"

#define VERSION_MIN 2
#define VERSION_MAX 10 // 10.5.50 透明视频&anchor

namespace mn {

static SceneDataDecoder* DecodeImages(SceneDataDecoder* data, MBuffer* buffer) {
    int count = buffer->ReadInt();
    PRINT_DATA_INT("images count", count);
    
    for (int i = 0; i < count; i++) {
        const char* url = buffer->ReadString();
        const char* ktx_android = buffer->ReadString();
        const char* ktx_ios = buffer->ReadString();
        data->image_url_idx_map_[url] = i;
        if (strlen(ktx_android) > 0) {
            data->image_url_idx_map_[ktx_android] = i;
            PRINT_DATA_STRING("image astc", ktx_android);
        }
        if (strlen(ktx_ios) > 0) {
            data->image_url_idx_map_[ktx_ios] = i;
            PRINT_DATA_STRING("image pvrtc", ktx_ios);
        }
        PRINT_DATA_STRING("image url", url);
    }
    data->images_datas_.resize(count, nullptr);
    
    return data;
}

static SceneDataDecoder* DecodeShapes(SceneDataDecoder* data, MBuffer* buffer) {
    int count = buffer->ReadInt();
    PRINT_DATA_INT("shapes count", count);

    // todo: 避免拷贝
    for (int i = 0; i < count; i++) {
        auto shape = new GeometryShapeData();
        int geometries_count = buffer->ReadInt();
        for (int j = 0; j < geometries_count; j++) {
            auto geometry = std::make_shared<ShapeGeometryData>();
            int segments_count = buffer->ReadInt();
            for (int k = 0; k < segments_count; k++) {
                int segment_length = buffer->ReadInt();
                std::vector<float> segment;
                for (int l = 0; l < segment_length; l++) {
                    segment.push_back(buffer->ReadFloat());
                }
                geometry->segments_.push_back(segment);
            }
            int points_count = buffer->ReadInt();
            for (int k = 0; k < points_count; k++) {
                int point_length = buffer->ReadInt();
                std::vector<float> point;
                for (int l = 0; l < point_length; l++) {
                    point.push_back(buffer->ReadFloat());
                }
                geometry->points_.push_back(point);
            }
            shape->geometries_.push_back(geometry);
        }
        data->shapes_.push_back(shape);
    }
    
    return data;
}

static SceneDataDecoder* DecodeFeatures(SceneDataDecoder* data, MBuffer* buffer) {
    DataTag tag;
    tag.val = buffer->ReadInt();
    
    while (tag.val != 0) {
        int buffer_idx = buffer->CurrentIdx();
        int buffer_len = buffer->ReadInt();
        MLOGD("DecodeFeature %d length %d", tag.i.idx, buffer_len);
        switch (tag.i.idx) {
            case 1:
            {
                int idx = buffer->ReadInt();
                while (idx != 0) {
                    std::string webp_url = buffer->ReadString();
                    data->image_url_idx_map_[webp_url] = idx - 1;
                    idx = buffer->ReadInt();
                }
                if (buffer->CurrentIdx() - buffer_len - 1 != buffer_idx) {
                    MLOGE("DecodeFeature fail %d %d %d", buffer->CurrentIdx(), buffer_len, buffer_idx);
                    return nullptr;
                }
            }
                break;
                
            case 2:
            {
                int idx = buffer->ReadInt();
                while (idx != 0) {
                    float x = buffer->ReadFloat();
                    float y = buffer->ReadFloat();
                    int template_type = buffer->ReadInt();
                    int bg_width = buffer->ReadInt();
                    int bg_height = buffer->ReadInt();
                    int var_image_id = buffer->ReadInt();
                    std::string var_image_name = buffer->ReadString();
                    idx -= 1;
                    if (template_type == 1) {
                        data->temp_image_info_[var_image_id].idx = idx;
                        auto& info = data->temp_image_info_[var_image_id];
                        info.x = x;
                        info.y = y;
                        info.expected_width = bg_width;
                        info.expected_height = bg_height;
                        data->image_url_idx_map_[var_image_name] = var_image_id;
                    }
                    idx = buffer->ReadInt();
                }
                if (buffer->CurrentIdx() - buffer_len - 1 != buffer_idx) {
                    MLOGE("DecodeFeature fail %d %d %d", buffer->CurrentIdx(), buffer_len, buffer_idx);
                    return nullptr;
                }
            }
                break;
                
            case 3:
            {
                int count = buffer->ReadInt();
                for (int i = 0; i < count; i++) {
                    int byte_len = buffer->ReadInt();
                    uint8_t* image_data = buffer->ReadUint8Array(byte_len);
                    data->images_datas_[i] = new ImageRawData(image_data, byte_len);
                    data->images_datas_[i]->need_free = false;
                }
            }
                break;
                
            case 4:
            {
                data->fangao_ = new FangaoData;
                int replace_count = buffer->ReadInt();
                for (int i = 0; i < replace_count; i++) {
                    data->fangao_->replace.push_back(
                        FangaoReplace(
                            buffer->ReadString(),
                            buffer->ReadFloat(),
                            buffer->ReadFloat(),
                            buffer->ReadFloat(),
                            buffer->ReadFloat()
                        )
                    );
                }
                int follow_count = buffer->ReadInt();
                for (int i = 0; i < follow_count; i++) {
                    data->fangao_->follow.push_back(
                        FangaoFollow(
                            buffer->ReadString(),
                            buffer->ReadString(),
                            buffer->ReadString()
                        )
                    );
                }
            }
                break;
                
            default:
                MLOGD("DecodeFeature skip");
                buffer->ReadFloatArray(buffer_len);
                break;
        }
        
        tag.val = buffer->ReadInt();
    }
    
    return data;
}

static SceneDataDecoder* InnerCreateSceneData(uint8_t* data_buffer) {
    SceneDataDecoder* data = new SceneDataDecoder();
    
    data->buffer_ = new MBuffer(data_buffer);
    auto buffer_ = data->buffer_;
    
    int version = buffer_->ReadInt();
    if (version < VERSION_MIN || version > VERSION_MAX) {
        MLOGE("version not match data: %d expect: %d~%d downgrade", version, VERSION_MIN, VERSION_MAX);
        delete data;
        return nullptr;
    }
    PRINT_DATA_INT("version", version);
    
    // 暂时没用
    const char* scene_version = buffer_->ReadString();
    PRINT_DATA_STRING("scene version", scene_version);
    
    data->composition_id_ = buffer_->ReadString();
    PRINT_DATA_STRING("comp id", data->composition_id_);
    
    int block = buffer_->ReadInt();
    while (block) {
        switch (block) {
            case 1:
            {
                int comp_count = buffer_->ReadInt();
                PRINT_DATA_INT("comp count", comp_count);
                for (int i = 0; i < comp_count; i++) {
                    auto comp = CompositionDecoder::Decode(buffer_);
                    if (!comp) {
                        delete data;
                        MLOGE("MNData decode failed");
                        return nullptr;
                    }
                    data->compositions_.push_back(comp);
                }
            }
                break;
            
            case 2:
                if (!DecodeImages(data, buffer_)) {
                    delete data;
                    MLOGE("MNData decode images failed");
                    return nullptr;
                }
                break;
                
            case 4:
                if (!DecodeShapes(data, buffer_)) {
                    delete data;
                    MLOGE("MNData decode shapes failed");
                    return nullptr;
                }
                break;
                
            case 6:
            {
                int length = buffer_->ReadInt();
                for (int i = 0; i < length; i++) {
                    MarsBinData bin;
                    bin.length = buffer_->ReadInt();
                    bin.data = buffer_->ReadUint8Array(bin.length);
                    buffer_->bins_.push_back(bin);
                }
            }
                break;
                
            case 100:
                if (!DecodeFeatures(data, buffer_)) {
                    delete data;
                    MLOGE("MNData decode features failed");
                    return nullptr;
                }
                break;
                
            default:
                MLOGD("block %d not support", block);
                return data;
        }
        
        block = buffer_->ReadInt();
    }
    MLOGD("InnerCreateSceneData finish %p %d", data, buffer_->CurrentIdx());
    
    return data;
}

SceneDataDecoder* SceneDataDecoderNew::CreateNew(uint8_t* data_buffer) {
    SceneDataDecoder* data = InnerCreateSceneData(data_buffer);
    
    if (data && !data->InitComposition()) {
        MLOGD("init scene data fail");
        delete data;
        return nullptr;
    }
    
    return data;
}

SceneDataDecoder* SceneDataDecoderNew::CreateNewByPath(const char* file_path) {
    if (file_path == nullptr || strlen(file_path) <= 0) {
        MLOGE("SceneDataDecoderNew::CreateNewByPath file path is null");
        return nullptr;
    }
    MLOGD("SceneDataDecoderNew::CreateNewByPath file path: %s", file_path);
    FILE* file = fopen(file_path, "rb");
    if (file == NULL) {
        return nullptr;
    }
    auto descriptor = fileno(file);
    struct stat stat_buf;
    if (fstat(descriptor, &stat_buf) == -1) {
        fclose(file);
        return nullptr;
    }
    size_t buffer_length = static_cast<size_t>(stat_buf.st_size);
    uint8_t* buffer = (uint8_t*)malloc(buffer_length);
    if (buffer == nullptr) {
        fclose(file);
        return nullptr;
    }
    static size_t MAX_BUFFER_LENGTH = 5 * 1024;
    for (size_t offset = 0; offset < buffer_length; ) {
        size_t len = buffer_length - offset;
        if (len > MAX_BUFFER_LENGTH) {
            len = MAX_BUFFER_LENGTH;
        }
        size_t n = fread(buffer + offset, 1, len, file);
        if (n == 0) {
            break;
        }
        offset += n;
    }
    fclose(file);

    return CreateNew(buffer);
}

SceneDataDecoder* SceneDataDecoderNew::CreateNewBySingleBinData(uint8_t* data_buffer, int data_buffer_length) {
    MarsNativeData* mars_data = MarsNativeData::CreateNew(data_buffer, data_buffer_length, true, nullptr);
    SceneDataDecoder* ret = nullptr;

    if (mars_data) {
        ret = SceneDataDecoderNew::CreateNew((uint8_t*) mars_data->GetSceneData());
        int length = mars_data->GetImageDataCount();
        for (int i = 0; i < length; i++) {
            MarsNativeImageData* image_data = mars_data->GetImageDataAt(i);
            if (!image_data || !image_data->buffer) {
                MLOGE("CreateNewBySingleBinData null image data");
                delete ret;
                delete mars_data;
                return nullptr;
            }
            uint8_t* data;
            int length;
            if (image_data->width > 0 && image_data->height > 0) {
                length = image_data->width * image_data->height * 4;
            } else if (image_data->buffer_length > 0) {
                length = image_data->buffer_length;
            } else {
                MLOGE("CreateNewBySingleBinData invalid buffer length: %d, size: %d %d", image_data->buffer_length, image_data->width, image_data->height);
                delete ret;
                delete mars_data;
                return nullptr;
            }
            if (!image_data->need_free) {
                data = (uint8_t*) malloc(length);
                if (!data) {
                    MLOGE("CreateNewBySingleBinData malloc buffer fail %d", length);
                    delete ret;
                    delete mars_data;
                    return nullptr;
                }
                memcpy(data, image_data->buffer, length);
            } else {
                data = (uint8_t*) image_data->buffer;
                image_data->need_free = false;
            }
            ImageRawData* raw_data;
            if (image_data->width > 0 && image_data->height > 0) {
                raw_data = new ImageRawData(data, image_data->width, image_data->height);
                raw_data->premultiply_alpha = image_data->premultiply_alpha;
            } else {
                raw_data = new ImageRawData(data, image_data->buffer_length);
            }
            ret->SetImageData(image_data->url, raw_data);
        }
        delete mars_data;
    }

    return ret;
}

SceneDataDecoder* SceneDataDecoderNew::CreateNewBySingleBinPath(const char* file_path) {
    if (file_path == nullptr || strlen(file_path) <= 0) {
        MLOGE("SceneDataDecoderNew::CreateNewBySingleBinPath file path is null");
        return nullptr;
    }
    MLOGE("SceneDataDecoderNew::CreateNewBySingleBinPath file path: %s", file_path);
    FILE* file = fopen(file_path, "rb");
    if (file == NULL) {
        return nullptr;
    }
    auto descriptor = fileno(file);
    struct stat stat_buf;
    if (fstat(descriptor, &stat_buf) == -1) {
        fclose(file);
        return nullptr;
    }
    size_t buffer_length = static_cast<size_t>(stat_buf.st_size);
    uint8_t* buffer = (uint8_t*)malloc(buffer_length);
    if (buffer == nullptr) {
        fclose(file);
        return nullptr;
    }
    static size_t MAX_BUFFER_LENGTH = 5 * 1024;
    for (size_t offset = 0; offset < buffer_length; ) {
        size_t len = buffer_length - offset;
        if (len > MAX_BUFFER_LENGTH) {
            len = MAX_BUFFER_LENGTH;
        }
        size_t n = fread(buffer + offset, 1, len, file);
        if (n == 0) {
            break;
        }
        offset += n;
    }
    fclose(file);

    return CreateNewBySingleBinData(buffer, buffer_length);
}

}
