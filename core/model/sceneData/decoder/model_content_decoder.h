//
//  model_content_decoder.h
//
//  Created by changxing on 2022/10/13.
//

#ifndef model_content_decoder_h
#define model_content_decoder_h

#include "model/sceneData/data/model_data.h"

namespace mn {

class ModelContentDecoder {
public:
    static ContentDataBase* DecodeMeshContent(MBuffer* buffer);
    
    static ContentDataBase* DecodeTreeContent(MBuffer* buffer);
    
    static ContentDataBase* DecodeSkyboxContent(MBuffer* buffer);
    
    static ContentDataBase* DecodeLightContent(MBuffer* buffer);
};

}

#endif /* model_content_decoder_h */
