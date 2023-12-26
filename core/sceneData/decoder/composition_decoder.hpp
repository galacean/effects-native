//
//  composition_decoder.hpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/6/3.
//

#ifndef composition_decoder_hpp
#define composition_decoder_hpp

#include "sceneData/scene_data_decoder.h"

namespace mn {

class CompositionDecoder {
public:
    static CompositionData* Decode(MBuffer* buffer);
};

}

#endif /* composition_decoder_hpp */
