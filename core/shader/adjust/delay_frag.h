//
//  delay_frag.h
//  MarsNative
//
//  Created by Zongming Liu on 2022/7/31.
//  Copyright © 2022 Alipay. All rights reserved.
//

#ifndef delay_frag_h
#define delay_frag_h

namespace mn {

const std::string DELAY_FRAG = R"(
    uniform sampler2D uLastSource;
    uniform vec4 uParams;//enabled blending
    vec4 filterMain(vec2 coord,sampler2D tex){
        vec4 c1 = texture2D(tex, coord);
        if (uParams.x < 1.){
            return c1;
        }
        vec4 c2 = texture2D(uLastSource, coord);
        return mix(c1, c2, uParams.y);
    }
)";

}


#endif /* delay_frag_h */
