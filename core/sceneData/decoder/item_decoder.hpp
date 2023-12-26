//
//  item_decoder.hpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/6/3.
//

#ifndef item_decoder_hpp
#define item_decoder_hpp

#include "sceneData/scene_data_decoder.h"

namespace mn {

class ItemDecoder {
public:
    static ItemData* Decode(MBuffer* buffer);
};

}

#endif /* item_decoder_hpp */
