//
//  utils.cpp
//
//  Created by Zongming Liu on 2022/4/8.
//

#include "utils.hpp"
#include "util/log_util.hpp"

namespace mn {

GLenum Utils::ConvertToGLType(BufferDataType type) {
    if (type == BufferDataType::UNSIGNED_BYTE) {
        return GL_UNSIGNED_BYTE;
    } else if (type == BufferDataType::BYTE) {
        return GL_BYTE;
    } else if (type == BufferDataType::UNSIGNED_SHORT) {
        return GL_UNSIGNED_SHORT;
    } else if (type == BufferDataType::SHORT) {
        return GL_SHORT;
    } else if (type == BufferDataType::UNSIGNED_INT) {
        return GL_UNSIGNED_INT;
    } else if (type == BufferDataType::B_INT) {
        return GL_INT;
    } else if (type == BufferDataType::FLOAT) {
        return GL_FLOAT;
    } else {
        MLOGE("Not Matched GLTYPE");
        return GL_FLOAT;
    }
}

size_t Utils::GetBytePerElementsByGLType(GLenum type) {
    size_t bpe = 0;
    
    if (type == GL_FLOAT) {
        bpe = sizeof(float);
    } else if (type == GL_INT) {
        bpe = sizeof(int);
    } else if (type == GL_UNSIGNED_INT) {
        bpe = sizeof(unsigned int);
    } else if (type == GL_SHORT) {
        bpe = sizeof(short);
    } else if (type == GL_UNSIGNED_SHORT) {
        bpe = sizeof(unsigned short);
    } else if (type == GL_UNSIGNED_BYTE) {
        bpe = 1;
    } else if (type == GL_BYTE) {
        bpe = 1;
    } else {
        MLOGE("GetBytePerElement not match type: %d", type);
        assert(0);
    }
    
    return bpe;
}


size_t Utils::SizeOfDataType(BufferDataType type) {
    if (type == BufferDataType::UNSIGNED_BYTE || type == BufferDataType::BYTE) {
        return 1;
    } else if (type == BufferDataType::UNSIGNED_SHORT || type == BufferDataType::SHORT) {
        return sizeof(short);
    } else if (type == BufferDataType::UNSIGNED_INT || type == BufferDataType::B_INT) {
        return sizeof(int);
    } else if (type == BufferDataType::FLOAT) {
        return sizeof(float);
    } else {
        return sizeof(float);
    }
}

GLenum Utils::ConverUniformDataType(UniformDataType type) {
    if (type == UniformDataType::INT) {
        return GL_INT;
    } else if (type == UniformDataType::INT_VEC2) {
        return GL_INT_VEC2;
    } else if (type == UniformDataType::INT_VEC3) {
        return GL_INT_VEC3;
    } else if (type == UniformDataType::INT_VEC4) {
        return GL_INT_VEC4;
    } else if (type == UniformDataType::FLOAT_VEC) {
        return GL_FLOAT;
    } else if (type == UniformDataType::FLOAT_VEC2) {
        return GL_FLOAT_VEC2;
    } else if (type == UniformDataType::FLOAT_VEC3) {
        return GL_FLOAT_VEC3;
    } else if (type == UniformDataType::FLOAT_VEC4) {
        return GL_FLOAT_VEC4;
    } else if (type == UniformDataType::FLOAT_MAT2) {
        return GL_FLOAT_MAT2;
    } else if (type == UniformDataType::FLOAT_MAT3) {
        return GL_FLOAT_MAT3;
    } else if (type == UniformDataType::FLOAT_MAT4) {
        return GL_FLOAT_MAT4;
    } else {
        MLOGE("Not Matched Uniform Data type");
        return GL_FLOAT;
    }
}

size_t Utils::ByteLengthOfUniformData(UniformDataType type) {
    if (type == UniformDataType::INT) {
        return 1 * sizeof(int);
    } else if (type == UniformDataType::INT_VEC2) {
        return 2 * sizeof(int);
    } else if (type == UniformDataType::INT_VEC3) {
        return 3 * sizeof(int);
    } else if (type == UniformDataType::INT_VEC4) {
        return 4 * sizeof(int);
    } else if (type == UniformDataType::FLOAT_VEC) {
        return 1 * sizeof(float);
    } else if (type == UniformDataType::FLOAT_VEC2) {
        return 2 * sizeof(float);
    } else if (type == UniformDataType::FLOAT_VEC3) {
        return 3 * sizeof(float);
    } else if (type == UniformDataType::FLOAT_VEC4) {
        return 4 * sizeof(float);
    } else if (type == UniformDataType::FLOAT_MAT2) {
        return 4 * sizeof(float);
    } else if (type == UniformDataType::FLOAT_MAT3) {
        return 9 * sizeof(float);
    } else if (type == UniformDataType::FLOAT_MAT4) {
        return 16 * sizeof(float);
    } else if (type == UniformDataType::SAMPLER_2D) {
        return 1 * sizeof(int);
    } else if (type == UniformDataType::SAMPLER_CUBE) {
        return 1 * sizeof(int);
    } else {
        MLOGE("Not Matched Uniform Data type");
        return 1 * sizeof(float);
    }
}


}
