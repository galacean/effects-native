//
//  m_geometry.hpp
//
//  Created by Zongming Liu on 2022/9/16.
//

#ifndef m_geometry_hpp
#define m_geometry_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include "ri/render/mars_geometry.hpp"

namespace mn {

class MGeometry {
    
public:
    
    MGeometry(std::shared_ptr<MarsGeometry> geometry);
    
    ~MGeometry();
        
    void SetHide(bool hide);
    
    std::shared_ptr<MarsGeometry> Geometry() {
        return geometry_;
    }
    
    bool HasPosition();
    
    bool HasNormals();
    
    bool HasTangents();
    
    bool HasUVCoord(int index);
    
    bool HasColor();
    
    bool HasJoints();
    
    bool HasWeight();
    
private:
  
    bool ExistAttributeName(const std::string& name);
    
    std::shared_ptr<MarsGeometry> geometry_;
    
    std::vector<std::string> attribute_names_;
    
};

}

#endif /* m_geometry_hpp */
