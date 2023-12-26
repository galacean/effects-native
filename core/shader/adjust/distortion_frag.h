//
//  distortion_frag.h
//  MarsNative
//
//  Created by Zongming Liu on 2022/8/24.
//  Copyright © 2022 Alipay. All rights reserved.
//

#ifndef distortion_frag_h
#define distortion_frag_h

namespace mn {
// todo: particle;

const std::string DISTORTION_FRAG = R"(
    uniform vec4 uWaveParams;
    uniform vec4 vWaveParams;
    vec4 filterMain(vec2 texCoord,sampler2D tex){
        vec2 vp = texCoord - uWaveParams.xy;
        float xx = dot(vp,uWaveParams.zw);
        float d = sin(vWaveParams.x * xx + vWaveParams.y) * vWaveParams.z;
        vec2 up = vec2(-uWaveParams.w,uWaveParams.z) * d;
        return texture2D(tex,clamp(texCoord + up,0.,1.));
    }
)";

}


#endif /* distortion_frag_h */
