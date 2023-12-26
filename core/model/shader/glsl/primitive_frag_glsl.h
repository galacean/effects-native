//
//  primitive_frag.h
//  PlayGround
//
//  Created by Zongming Liu on 2022/9/23.
//

#ifndef primitive_frag_h
#define primitive_frag_h

namespace mn {

const std::string PRIMITIVE_FRAG = R"(

precision mediump float;

#define vsIn attribute
#define vsOut varying
#define fsIn varying
#define fsOut varying

fsIn vec2 v_UVCoord1;
uniform sampler2D u_BaseColorSampler;

void main(void)
{
    gl_FragColor = texture2D(u_BaseColorSampler, v_UVCoord1);
}

)";

}


#endif /* primitive_frag_h */
