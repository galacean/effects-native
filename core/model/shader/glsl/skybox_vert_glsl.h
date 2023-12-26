//
//  skybox_vert_glsl.h
//
//  Created by Zongming Liu on 2023/2/9.
//

#ifndef skybox_vert_glsl_h
#define skybox_vert_glsl_h

namespace mn {

const std::string SKY_BOX_VERT_GLSL = R"(

precision highp float;

#define FEATURES

#define vsIn attribute
#define vsOut varying
#define fsIn varying
#define fsOut varying

vsIn vec3 a_Position;
vsOut vec3 v_CameraDir;


uniform mat4 u_InvViewProjectionMatrix;

void main(){
  vec4 dir = u_InvViewProjectionMatrix * vec4(a_Position.xy, 1, 1);
  v_CameraDir = normalize(dir.xyz / dir.w);

  gl_Position = vec4(a_Position.xy, 0.99999, 1);
}

)";

}


#endif /* skybox_vert_glsl_h */
