//
//  copy_frag.h
//  MarsNative
//
//  Created by Zongming Liu on 2022/7/31.
//  Copyright © 2022 Alipay. All rights reserved.
//

#ifndef copy_frag_h
#define copy_frag_h

namespace mn {

const std::string COPY_FRAG = R"(
    uniform vec2 uFilterSourceSize;
    uniform sampler2D uFilterSource;
    vec4 filterMain(vec2 texCoord,sampler2D tex){
        #ifdef INTERPOLATION
        vec2 texInPixel = texCoord * uFilterSourceSize;
        vec2 coordMin = floor(texInPixel);
        vec2 pp = texInPixel - coordMin;
        vec4 fCoord = vec4(coordMin/uFilterSourceSize,(coordMin+vec2(1.))/uFilterSourceSize);
        vec4 cLT = texture2D(uFilterSource,vec2(fCoord.x,fCoord.w));
        vec4 cLB = texture2D(uFilterSource,vec2(fCoord.x,fCoord.y));
        vec4 cRT = texture2D(uFilterSource,vec2(fCoord.z,fCoord.w));
        vec4 cRB = texture2D(uFilterSource,vec2(fCoord.z,fCoord.y));
        return mix(mix(cLB,cRB,pp.x),mix(cLT,cRT,pp.x),pp.y);
        #else
        return texture2D(uFilterSource,texCoord);
        #endif
    }
)";

}




#endif /* copy_frag_h */
