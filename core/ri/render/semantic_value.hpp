//
//  semantic_value.hpp
//
//  Created by Zongming Liu on 2022/4/12.
//

#ifndef semantic_value_hpp
#define semantic_value_hpp

#include <stdio.h>

const std::string SEMANTIC_PRE_COLOR_ATTACHMENT_0 = "PRE_COLOR_0";
const std::string SEMANTIC_PRE_COLOR_ATTACHMENT_SIZE_0 = "PRE_COLOR_SIZE_0";
const std::string SEMANTIC_MAIN_PRE_COLOR_ATTACHMENT_0 = "PRE_MAIN_COLOR_0";
const std::string SEMANTIC_MAIN_PRE_COLOR_ATTACHMENT_SIZE_0 = "PRE_MAIN_COLOR_SIZE_0";

namespace mn {

class RenderFrame;

class RenderPass;

class MarsMesh;

class Camera;

class RenderState {
    
public:
    
    RenderState() { }
    
    RenderFrame *current_render_frame = nullptr;

    RenderPass *current_render_pass = nullptr;
    
    MarsMesh *current_mesh = nullptr;
    
    Camera *current_camera = nullptr;
    
};

class SemanticValue {

public:
    
    virtual ~SemanticValue() {}
    
    virtual void GetSemantiveValue(const RenderState& state, std::shared_ptr<UniformValue> value) { };

};

}

#endif /* semantic_value_hpp */
