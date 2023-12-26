//
//  particle_content_decoder.hpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/6/5.
//

#ifndef particle_content_decoder_hpp
#define particle_content_decoder_hpp

#include "sceneData/scene_data_decoder.h"

namespace mn {

class ParticleContentDecoder {
public:
    static ParticleContentData* Decode(MBuffer* buffer);
};

}

#endif /* particle_content_decoder_hpp */
