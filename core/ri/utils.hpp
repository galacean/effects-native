//
//  utils.hpp
//
//  Created by Zongming Liu on 2022/4/8.
//

#ifndef utils_hpp
#define utils_hpp

#include <stdio.h>
#include "ri/gl.h"
#include "ri/render_base.hpp"

namespace mn {

class Utils {
      
public:
    
    static GLenum ConvertToGLType(BufferDataType type);
        
    static size_t GetBytePerElementsByGLType(GLenum type);
    
    static size_t SizeOfDataType(BufferDataType type);
    
    static GLenum ConverUniformDataType(UniformDataType type);
    
    static size_t ByteLengthOfUniformData(UniformDataType type);
            
};

}

#endif /* utils_hpp */
