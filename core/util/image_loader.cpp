#include "image_loader.h"

namespace mn {

// https://www.khronos.org/registry/KTX/specs/1.0/ktxspec_v1.html#:~:text=KTX%E2%84%A2%20is%20a%20format,object%20from%20the%20file%20contents.
static uint8_t s_ktx_identifier[12] = {0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A};

bool ImageLoader::IsKtxImage(const uint8_t* raw_data, size_t byte_length) {
    if (!raw_data || byte_length < 12) {
        return false;
    }
    for (int i = 0; i < 12; i++) {
        if (raw_data[i] != s_ktx_identifier[i]) {
            return false;
        }
    }
    return true;
}

}
