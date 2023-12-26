//
//  bloom_t_frag.h
//  MarsNative
//
//  Created by Zongming Liu on 2022/8/26.
//  Copyright © 2022 Alipay. All rights reserved.
//

#ifndef bloom_t_frag_h
#define bloom_t_frag_h

namespace mn {

const std::string BLOOM_T_FRAG = R"(
   uniform vec4 uColorThreshold;//rgb weights

   vec4 filterMain(vec2 coord,sampler2D tex){
       vec4 c1 = texture2D(tex, coord);
       vec3 color = c1.rgb * c1.a;
       vec3 s = step(uColorThreshold.xyz, color);
       float m = min(s.r+s.g+s.b, 1.);
       return c1 * m;
   }
)";


}


#endif /* bloom_t_frag_h */
