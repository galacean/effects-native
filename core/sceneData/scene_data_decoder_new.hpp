//
//  scene_data_decoder_new.hpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/6/3.
//

#ifndef scene_data_decoder_new_hpp
#define scene_data_decoder_new_hpp

#include "scene_data_decoder.h"

namespace mn {

class SceneDataDecoderNew {
public:
    static SceneDataDecoder* CreateNew(uint8_t* data_buffer);

    static SceneDataDecoder* CreateNewByPath(const char* file_path);

    static SceneDataDecoder* CreateNewBySingleBinData(uint8_t* data_buffer, int data_buffer_length);

    static SceneDataDecoder* CreateNewBySingleBinPath(const char* file_path);
};

}

#endif /* scene_data_decoder_new_hpp */
