//
//  model_data.cpp
//
//  Created by changxing on 2022/11/1.
//

#include "model_data.h"
#include "util/image_loader.h"

namespace mn {

std::shared_ptr<MarsTexture> GLTFTextureData::ToMarsTexture(Composition* composition) {
    if (cube_mipmaps) {
        if (!cube_mipmaps->texture) {
            std::vector<ImageData*> image_data_list;
            TextureOption opt(2, 2, TextureSourceType::DATA);
            opt.source_type = TextureSourceType::MIPMAPS;
            opt.texture_info.target = GL_TEXTURE_CUBE_MAP;
            for (int i = 0; i < cube_mipmaps->datas.size(); i += 6) {
                TextureRawData level_cube_data;
                for (int j = 0; j < 6; j++) {
                    BinaryData* bin = cube_mipmaps->datas[i + j];
                    ImageRawData raw_data((uint8_t*) bin->data_ref, bin->length);
                    raw_data.need_free = false;
                    auto img = ImageLoader::ParseImageData(&raw_data);
                    if (!img) {
                        MLOGE("GLTFTextureData decode cube image fail");
                        return nullptr;
                    }
                    image_data_list.push_back(img);
                    level_cube_data.width = img->width;
                    level_cube_data.height = img->height;
                    level_cube_data.cube_data.push_back(img->data);
                    opt.premultiply_alpha = img->premultiply_alpha;
                }
                opt.raw_mipmap_data.mipmaps.push_back(level_cube_data);
            }
            opt.texture_info.min_filter = min_filter->val;
            opt.texture_info.mag_filter = mag_filter->val;
            opt.texture_info.wrap_t = wrap_t->val;
            opt.texture_info.wrap_s = wrap_s->val;
            cube_mipmaps->texture = std::make_shared<MarsTexture>(composition->renderer_, opt);
            for (int i = 0; i < image_data_list.size(); i++) {
                delete image_data_list[i];
            }
        }
        return cube_mipmaps->texture;
    }
    if (!image_file_id) {
        MLOGE("GLTFTextureData dont have image file id");
        return nullptr;
    }
    
    std::shared_ptr<MarsTexture> texture = composition->GetTextureContainerById(image_file_id->val);
    GLTexture* inner_texture = texture->GetInnerTexture();
    if (inner_texture) {
        
        // todo: 新春之后，在处理texture数据的时候，需要根据json中的纹理属性来设置texture filter/wrap; specificaiton的数据描述也有问题；
        texture->GetInnerTexture()->GetGLTextureInfo().wrap_s = GL_REPEAT;
        texture->GetInnerTexture()->GetGLTextureInfo().wrap_t = GL_REPEAT;
        inner_texture->Bind();
        inner_texture->SetTextureFilters();
    }
    
    return composition->GetTextureContainerById(image_file_id->val);
}

}
