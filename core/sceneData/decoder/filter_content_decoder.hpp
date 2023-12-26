//
//  filter_content_decoder.hpp
//  MarsNative
//
//  Created by Zongming Liu on 2022/8/2.
//  Copyright © 2022 Alipay. All rights reserved.
//

#ifndef filter_content_decoder_hpp
#define filter_content_decoder_hpp

#include <stdio.h>
#include "sceneData/data/common_properties.h"

namespace mn {

class FilterContentDecoder {
  
public:
    
    static ItemFilterData* GaussionFilterContentDecode(MBuffer* buffer);
    
    static ItemFilterData* DelayFilterContentDecode(MBuffer* buffer);

    static ItemFilterData* BloomFilterContentDecode(MBuffer* buffer);
    
    static ItemFilterData* DistortionFilterContentDecode(MBuffer* buffer);
    
};

}

#endif /* filter_content_decoder_hpp */
