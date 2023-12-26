//
//  common_properties_decoder.hpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/6/4.
//

#ifndef common_properties_decoder_hpp
#define common_properties_decoder_hpp

#include "sceneData/scene_data_decoder.h"

namespace mn {

class CommonPropertiesDecoder {
public:
    static ItemRendererOptionsData* DecodeRendererOptions(MBuffer* buffer);

    static ItemSizeOverLifetimeData* DecodeSizeOLT(MBuffer* buffer);

    static ItemRotateOverLifetimeData* DecodeRotateOLT(MBuffer* buffer);

    static ItemColorOverLifetimeData* DecodeColorOLT(MBuffer* buffer);

    static ItemPositionOverLifetimeData* DecodePositionOLT(MBuffer* buffer);

    static ItemTextureSheetAnimationData* DecodeTextureSheetAni(MBuffer* buffer);
    
    static ItemFilterData* DecodeFilterOption(MBuffer* buffer);

    static MNumberOrValue* DecodeNumberExpression(MBuffer* buffer);

    static bool DecodeVec3Expression(ParticleContentEmitterTransformData* position, MBuffer* buffer);
    
};

}

#endif /* common_properties_decoder_hpp */
