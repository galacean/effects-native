#ifndef sprite_content_decoder_h_
#define sprite_content_decoder_h_

#include "sceneData/scene_data_decoder.h"

namespace mn {

class SpriteContentDecoder {
public:
    static SpriteContentData* Decode(MBuffer* buffer);
};

}

#endif
