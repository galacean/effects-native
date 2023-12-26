//
//  m_material_base.hpp
//
//  Created by Zongming Liu on 2022/9/16.
//

#ifndef m_material_base_hpp
#define m_material_base_hpp

#include <stdio.h>
#include <string>
#include "model/common.hpp"
#include "ri/render/mars_material.hpp"

namespace mn {

class MMaterialBase {
    
public:
    
    MMaterialBase();
    
    MMaterialBase(std::shared_ptr<MarsMaterialOptions> material_options);
    
    virtual ~MMaterialBase();
    
    void Build(const std::vector<std::string> feature_list);
    
    virtual void SetUpUniforms(std::shared_ptr<MarsMaterial> material);
    
    virtual void UpdateUniforms(std::shared_ptr<MarsMaterial> material);
    
    virtual void GetShaderFeatures(std::vector<std::string>& feature_list);
    
    std::shared_ptr<MaterialRenderStates> GetRenderState();
    
    const std::string& VertexShader() {
        return vertex_shader_;
    }
    
    const std::string& FragmentShader() {
        return frag_shader_;
    }
    
    MarsMaterialBlending GetBlendMode();
       
    MarsMaterialSide GetFaceSideMode();
       
    MarsMaterialType GetMaterialType();
    
    bool IsOpaque() {
        return blend_mode_ == MarsMaterialBlending::OPAQUE;
    }
    
    bool IsMasked() {
        return blend_mode_ == MarsMaterialBlending::MASKED;
    }
    
    bool IsTranslucent() {
        return blend_mode_ == MarsMaterialBlending::TRANSLUCENT;
    }
    
    bool IsAdditive() {
        return blend_mode_ == MarsMaterialBlending::ADDITIVE;
    }
    
protected:
        
    MarsMaterialType type_;
    
    MarsMaterialBlending blend_mode_;
    
    MarsMaterialSide side_mode_;
    
    std::string vertex_shader_;
    
    std::string frag_shader_;
    
    std::shared_ptr<MarsTexture> base_color_texture_;
    
    Vec4 base_color_factor_;
    
    std::string name_;
    
};

}

#endif /* m_material_base_hpp */
