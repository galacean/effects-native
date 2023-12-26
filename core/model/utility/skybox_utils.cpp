//
//  skybox_utils.cpp
//
//  Created by Zongming Liu on 2023/2/10.
//

#include "skybox_utils.hpp"

namespace mn {

std::shared_ptr<MarsMesh> SkyboxUtils::CreateSkyboxMesh(const std::string& name, MarsRenderer* renderer, std::shared_ptr<MMaterialSkybox> skybox_material) {
    
    std::string fragment_shader = skybox_material->FragmentShader();
    std::string vertex_shader = skybox_material->VertexShader();
    
    
    std::shared_ptr<MarsMesh> mesh = std::make_shared<MarsMesh>(renderer, name);
    
    MaterialOptions mars_materail_option(skybox_material->GetRenderState());
    mars_materail_option.shader.fragment = fragment_shader;
    mars_materail_option.shader.vertex = vertex_shader;
    
    std::shared_ptr<MarsMaterial> material = std::make_shared<MarsMaterial>(renderer, mars_materail_option);
    mesh->SetMaterial(material);
    
    // Geometry
    float plane_vertex_array[48] = {
        -1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
         1.0 -1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0,
         1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0,
         -1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
         1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0,
         -1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0
    };
    
    std::shared_ptr<MarsGeometry> geometry = std::make_shared<MarsGeometry>(renderer, 6, 0);
    
    int bpe = sizeof(float);
    geometry->SetAttributeInfo("a_Position", AttributeWithData(3, bpe * 8, 0));
    geometry->SetAttributeInfo("a_UV1", AttributeWithData(2, bpe * 8, 3 * bpe, "a_Position"));
    geometry->SetAttributeInfo("a_Normal", AttributeWithData(3, bpe * 8, bpe * 5, "a_Position"));
    geometry->SetAttributeData("a_Position", (uint8_t*)plane_vertex_array,  bpe * 48);
    mesh->SetGeometry(geometry);
    return mesh;
}

}
