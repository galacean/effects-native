#ifndef interact_content_decoder_h_
#define interact_content_decoder_h_

#include "sceneData/scene_data_decoder.h"

namespace mn {

class InteractContentDecoder {
public:
    static InteractContentData* Decode(MBuffer* buffer);
};

}

#endif
