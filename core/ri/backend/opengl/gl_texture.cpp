//
//  gl_texture.cpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#include "gl_texture.hpp"

#include "ri/render_base.hpp"
#include "ri/backend/opengl/gl_gpu_renderer.hpp"

namespace mn {

GLTexture::GLTexture(GLGPURenderer *renderer, const TextureOption& texture_option)
                    : renderer_(renderer) {
    
    width_ = texture_option.raw_data.width;
    height_ = texture_option.raw_data.height;
    texture_ = renderer_->CreateGLTexture();
    texture_info_ = texture_option.texture_info;
    source_type_ = texture_option.source_type;
    premultiply_alpha_ = texture_option.premultiply_alpha;
    
    if (source_type_ == TextureSourceType::MIPMAPS) {
        UpdateMipMap(texture_option.raw_mipmap_data);
    } else {
        this->Update(texture_option.raw_data);
    }
}

GLTexture::~GLTexture() {
    if (texture_) {
        OpenGLApi::DeleteTexture(texture_);
    }
}

void GLTexture::Bind() {
    this->renderer_->State().BindTexture(texture_info_.target, texture_);
}

void GLTexture::Update(const TextureRawData& source_option) {
    const GLState& state = this->renderer_->State();
    GLenum type = texture_info_.type;
    
    if (type == GL_HALF_FLOAT) {
        texture_info_.internal_format = GL_RGBA16F;
    } else if (type == GL_FLOAT) {
        texture_info_.internal_format = GL_RGBA32F;
    }
    
    GLint origin_texture;
    GL_CHECK(glGetIntegerv(GL_TEXTURE_BINDING_2D, &origin_texture));
    this->Bind();
    if (source_type_ == TextureSourceType::FRAMEBUFFER) {
        width_ = source_option.width;
        height_ = source_option.height;
        
        if (width_ > 0 && height_ > 0) {
            OpenGLApi::TexImage2D(texture_info_.target, 0, texture_info_.internal_format, width_, height_, 0, texture_info_.format, texture_info_.type, NULL);
        } else {
            MLOGE("GLTexture frameBuffer update failed");
        }
    } else if (source_type_ == TextureSourceType::DATA || source_type_ == TextureSourceType::EMPTY) {
        width_ = source_option.width;
        height_ = source_option.height;
        
        GLenum target = texture_info_.target;
        if (target == GL_TEXTURE_CUBE_MAP) {
            if (source_option.cube_data.size() == 6) {
                for (size_t i=0; i<source_option.cube_data.size(); i++) {
                    GLenum cube_target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
                    OpenGLApi::TexImage2D(cube_target, 0, texture_info_.internal_format, width_, height_, 0, texture_info_.format, texture_info_.type, source_option.cube_data[i]);
                }
            } else {
                assert(0);
            }
        } else {
            OpenGLApi::TexImage2D(texture_info_.target, 0, texture_info_.internal_format, width_, height_, 0, texture_info_.format, texture_info_.type, source_option.data);
        }
    } else if (source_type_ == TextureSourceType::COMPRESSED) {
        this->GetCompressTextureOptions(source_option);
        if (width_ > 0 && height_ > 0 && source_option.data != nullptr) {
            OpenGLApi::CompressedTexImage2D(texture_info_.target, 0, texture_info_.internal_format, width_, height_, 0, texture_info_.byte_length, source_option.data + texture_info_.data_offset);
        } else {
            MLOGE("GLTexture compressed update failed");
        }
    } else {
        MLOGE("GLTexture update not support source type");
        assert(0);
    }
    
    this->SetTextureFilters();
    this->renderer_->State().BindTexture(GL_TEXTURE_2D, origin_texture);
}


void GLTexture::UpdateMipMap(const TextureMipmapRawData& mip_map_option) {
    
    GLint origin_texture;
    GL_CHECK(glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &origin_texture));
    
    this->Bind();
    if (source_type_ == TextureSourceType::MIPMAPS) {
        if (texture_info_.target == GL_TEXTURE_2D) {
            for (size_t i=0; mip_map_option.mipmaps.size(); i++) {
                auto& raw_data = mip_map_option.mipmaps[i];
                OpenGLApi::TexImage2D(texture_info_.target, i, texture_info_.internal_format, raw_data.width, raw_data.height, 0, texture_info_.format, texture_info_.type, raw_data.data);
                if (i == 0) {
                    width_ = raw_data.width;
                    height_ = raw_data.height;
                }
            }
            
        } else if (texture_info_.target == GL_TEXTURE_CUBE_MAP) {
            for (size_t i=0; i<mip_map_option.mipmaps.size(); i++) {
                auto& raw_data = mip_map_option.mipmaps[i];
                if (raw_data.cube_data.size() != 6) {
                    MLOGE("Update MipMap Failed Invalid Data");
                    assert(0);
                }
                
                for (size_t j=0; j<raw_data.cube_data.size(); j++) {
                    GLenum cube_target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + j;
                    OpenGLApi::TexImage2D(cube_target, i, texture_info_.internal_format, raw_data.width, raw_data.height, 0, texture_info_.format, texture_info_.type, raw_data.cube_data[j]);
                }
                
                if (i == 0) {
                    width_ = raw_data.width;
                    height_ = raw_data.height;
                }
            }
        } else {
            assert(0);
        }
    } else {
        assert(0);
    }
    
    this->SetTextureFilters();
    this->renderer_->State().BindTexture(GL_TEXTURE_CUBE_MAP, origin_texture);


}


void GLTexture::OffloadData() {
    // todo:
}

void GLTexture::SetTextureFilters() {
    OpenGLApi::TexParameteri(texture_info_.target, GL_TEXTURE_MIN_FILTER, texture_info_.min_filter);
    OpenGLApi::TexParameteri(texture_info_.target, GL_TEXTURE_MAG_FILTER, texture_info_.mag_filter);
    OpenGLApi::TexParameteri(texture_info_.target, GL_TEXTURE_WRAP_S, texture_info_.wrap_s);
    OpenGLApi::TexParameteri(texture_info_.target, GL_TEXTURE_WRAP_T, texture_info_.wrap_t);
}

void GLTexture::GetCompressTextureOptions(const TextureRawData& texture_option) {
    if (this->CheckKTXValid((uint8_t *) texture_option.data)) {
        uint32_t* header_ptr = (uint32_t *)texture_option.data + 3;
        uint32_t endianness = header_ptr[0];
        
        bool litter_endian = endianness == 0x04030201;
        
        uint32_t gl_type  = header_ptr[1];
        uint32_t gl_type_size = header_ptr[2];
        uint32_t gl_format = header_ptr[3];
        uint32_t gl_inter_format = header_ptr[4];
        uint32_t gl_base_internal_format = header_ptr[5];
        uint32_t pixel_width = header_ptr[6];
        uint32_t pixel_height = header_ptr[7];
        uint32_t pixel_depth = header_ptr[8];
        uint32_t number_of_array_elements = header_ptr[9];
        uint32_t number_of_face = header_ptr[10];
        uint32_t number_of_mipmap_level = header_ptr[11];
        uint32_t bytes_of_key_value_data = header_ptr[12];
        
        if (pixel_width == 0 || pixel_height == 0) {
            MLOGE("only 2d textures currently support");
            return ;
        }
        
        if (number_of_array_elements != 0) {
            MLOGE("texture arrays not currently supported");
            return ;
        }
        
        uint32_t image_size = *(header_ptr + 13 + bytes_of_key_value_data/4);
        if (gl_type == 0) {
            texture_info_.type = gl_type;
            this->width_ = pixel_width;
            this->height_ = pixel_height;
            texture_info_.internal_format = gl_inter_format;
            texture_info_.format = gl_format;
            texture_info_.byte_length = image_size;
            texture_info_.data_offset = (12 + 13 * 4 + bytes_of_key_value_data + 4);
        }
    }
}

bool GLTexture::CheckKTXValid(uint8_t *data) {
    if (data) {
        if (data[ 0 ] != 0xab ||
            data[ 1 ] != 0x4b ||
            data[ 2 ] != 0x54 ||
            data[ 3 ] != 0x58 ||
            data[ 4 ] != 0x20 ||
            data[ 5 ] != 0x31 ||
            data[ 6 ] != 0x31 ||
            data[ 7 ] != 0xbb ||
            data[ 8 ] != 0x0d ||
            data[ 9 ] != 0x0a ||
            data[ 10 ] != 0x1a ||
            data[ 11 ] != 0x0a) {
            MLOGE("KTX Failed not valid");
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}

}
