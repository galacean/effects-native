#ifndef MN_SCENE_DATA_DECODER_H_
#define MN_SCENE_DATA_DECODER_H_

#include <stdint.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "data/composition.h"
#include "data/fangao.h"
#include "util/log_util.hpp"

#ifdef BUILD_PLAYGROUND
class TestSpriteCombineUtil;
#endif

namespace mn {
class PluginSystem;
}

struct ImageRawData {
    
    // ios, android ktx
    ImageRawData(uint8_t* raw_data, size_t len ) : data(raw_data), byte_length(len) {}

    // android普通纹理
    ImageRawData(u_int8_t* raw_data, int w, int h) : data(raw_data), width(w), height(h), decoded(true) {}
    
    virtual ~ImageRawData() {
        if (data && need_free) {
            free(data);
        }
    }
    
    uint8_t* data = nullptr;
    
    size_t byte_length = 0;

    int width = 0;

    int height = 0;
    
    bool decoded = false;
    
    bool need_free = true;

    bool premultiply_alpha = false;
    
    bool is_video = false;
    
};

struct TemplateImageV1 {
    float x = 0;
    float y = 0;
    int expected_width = 0;
    int expected_height = 0;
    int idx = -1;
};

namespace mn {

// todo: rename
class SceneDataDecoder {
public:
    SceneDataDecoder() {}
    
    ~SceneDataDecoder();
    
    bool InitComposition();

    CompositionData* GetComposition();
    
    void SetImageData(const std::string& image_url, ImageRawData* image_data);
    
    void GetImageExpectedSize(int& width, int& height, const std::string& image_url);
    
    void SetAnimationLoop(bool looping);

    size_t GetImageCount() const {
        return images_datas_.size();
    }

    ImageRawData* GetImageData(int idx) {
        if (idx >= 0 && idx < images_datas_.size()) {
            return images_datas_[idx];
        }
        return nullptr;
    }

    void ReleaseImageData();

public:
    MBuffer* buffer_ = nullptr;

    const char* composition_id_ = nullptr;
    std::vector<CompositionData*> compositions_;

    std::unordered_map<std::string, int> image_url_idx_map_;
    
    std::unordered_map<int, TemplateImageV1> temp_image_info_;
    
    // 暂时存储图片数据，初始化texture完成之后，销毁掉
    std::vector<ImageRawData*> images_datas_;

    std::vector<GeometryShapeData*> shapes_;
    
    FangaoData* fangao_ = nullptr;
    
    bool valid_ = true;
};

}

#endif
