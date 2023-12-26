#ifndef MN_IMAGE_LOADER_H_
#define MN_IMAGE_LOADER_H_

#include <string>
#include <vector>
#include <stdlib.h>

struct ImageRawData;

namespace mn {

class ImageData {
public:
    ImageData() {}
    
    ~ImageData() {
        if (data) {
            free(data);
        }
    }
    
    uint8_t* data = nullptr;
    int width = 0;
    int height = 0;
    bool premultiply_alpha = false;
};

class ImageLoader {
public:
    static ImageData* ParseImageData(ImageRawData* raw_data);
    
    static bool IsKtxImage(const uint8_t* raw_data, size_t byte_length);
};



}

#endif
