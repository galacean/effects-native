//
//  plugin_content_decoder.h
//  PlayGroundIOS
//
//  Created by changxing on 2022/9/15.
//

#ifndef plugin_content_decoder_h
#define plugin_content_decoder_h

#include "sceneData/scene_data_decoder.h"

namespace mn {

class PluginContentDecoder {
public:
    static PluginContentData* Decode(MBuffer* buffer);
};

}

#endif /* plugin_content_decoder_h */
