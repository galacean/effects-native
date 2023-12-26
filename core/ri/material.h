#ifndef MN_RENDER_MATERIAL_H_
#define MN_RENDER_MATERIAL_H_

#include "ri/backend/opengl/gl_material.hpp"
#include "util/image_loader.h"

namespace mn {

class MaterialUtil {
public:
    static int SetMtlBlending(BLEND_MODE_TYPE blending, MaterialRenderStates& mtl) {
        mtl.blend_equation_alpha = std::make_shared<MUint>(GL_FUNC_ADD);
        mtl.blend_equation_rgb = std::make_shared<MUint>(GL_FUNC_ADD);
        mtl.blending = std::make_shared<MBool>(true);
        if (blending == BLEND_MODE_ADDITIVE || blending == BLEND_MODE_SUBTRACT) {
            mtl.blend_src = std::make_shared<MUint>(GL_ONE);
            mtl.blend_dst = std::make_shared<MUint>(GL_ONE);
            mtl.blend_src_alpha = std::make_shared<MUint>(GL_ONE);
            mtl.blend_dst_alpha = std::make_shared<MUint>(GL_ONE);
            if (blending == BLEND_MODE_SUBTRACT) {
                mtl.blend_equation_alpha = std::make_shared<MUint>(GL_FUNC_REVERSE_SUBTRACT);
                mtl.blend_equation_rgb = std::make_shared<MUint>(GL_FUNC_REVERSE_SUBTRACT);
                mtl.blend_src_alpha = std::make_shared<MUint>(GL_ZERO);
                return 1;
            }
            return 1;
        } else if (blending == BLEND_MODE_LUMINANCE_ADDITIVE) {
            mtl.blend_src_alpha = std::make_shared<MUint>(GL_ONE);
            mtl.blend_src = std::make_shared<MUint>(GL_ONE);
            mtl.blend_dst_alpha = std::make_shared<MUint>(GL_ONE);
            mtl.blend_dst = std::make_shared<MUint>(GL_ONE);
            return 2;
        } else if (blending == BLEND_MODE_MULTIPLY) {
            mtl.blend_src = std::make_shared<MUint>(GL_DST_COLOR);
            mtl.blend_dst = std::make_shared<MUint>(GL_ONE_MINUS_SRC_ALPHA);
        } else if (blending == BLEND_MODE_ADD_LIGHT) {
            mtl.blend_src = std::make_shared<MUint>(GL_DST_COLOR);
            mtl.blend_dst = std::make_shared<MUint>(GL_DST_ALPHA);
            mtl.blend_src_alpha = std::make_shared<MUint>(GL_ZERO);
            mtl.blend_dst_alpha = std::make_shared<MUint>(GL_ONE);
            return 1;
        } else if (blending == BLEND_MODE_LIGHT) {
            mtl.blend_src = std::make_shared<MUint>(GL_DST_COLOR);
            mtl.blend_dst = std::make_shared<MUint>(GL_ZERO);
            mtl.blend_src_alpha = std::make_shared<MUint>(GL_ZERO);
            mtl.blend_dst_alpha = std::make_shared<MUint>(GL_ONE);
            return 1;
        } else {
            mtl.blend_src = std::make_shared<MUint>(GL_ONE);
            mtl.blend_dst = std::make_shared<MUint>(GL_ONE_MINUS_SRC_ALPHA);
            mtl.blend_src_alpha = std::make_shared<MUint>(GL_ONE);
            mtl.blend_dst_alpha = std::make_shared<MUint>(GL_ONE_MINUS_SRC_ALPHA);
            if (blending == BLEND_MODE_LUMINANCE_ALPHA) {
                return 3;
            }
            return 1;
        }
        return 0;
    }
    
    static void SetMtlStencil(MASK_MODE_TYPE mask_mode, int mask, MaterialRenderStates& mtl) {
        if (mask_mode == MASK_MODE_NONE || !mask_mode) {
            mtl.stencil_test = std::make_shared<MBool>(false);
        } else {
            mtl.stencil_test = std::make_shared<MBool>(true);
            mtl.stencil_op = std::make_shared<RENDER_STATE_STENCIL_OP>();
            mtl.stencil_op->fail = GL_KEEP;
            mtl.stencil_op->zfail = GL_KEEP;
            mtl.stencil_op->zpass = GL_KEEP;
            mtl.stencil_func = std::make_shared<RENDER_STATE_STENCIL_FUNC>();
            mtl.stencil_func->func = GL_ALWAYS;
            mtl.stencil_func->ref = mask;
            mtl.stencil_func->mask = 0xFF;
            mtl.stencil_mask = std::make_shared<MUint>(0xFF);
            if (mask_mode == MASK_MODE_WRITE_MASK) {
                mtl.stencil_func->func = GL_ALWAYS;
                mtl.stencil_op->zpass = GL_REPLACE;
            } else if (mask_mode == MASK_MODE_READ_INVERT_MASK) {
                mtl.stencil_func->func = GL_NOTEQUAL;
            } else if (mask_mode == MASK_MODE_READ_MASK) {
                mtl.stencil_func->func = GL_EQUAL;
            }
          }
    }
    
    static std::shared_ptr<MarsTexture> CreateEmptyTextureContainer(MarsRenderer* renderer) {
        uint8_t* buffer = new uint8_t[4];
        for (int i = 0; i < 4; i++) buffer[i] = 255;
        
        ImageData data;
        data.width = 1;
        data.height = 1;
        data.data = buffer;
        
        return CreateDataTexture(&data, renderer);
    }
    
    static std::shared_ptr<MarsTexture> CreateDataTexture(ImageData* image_data, MarsRenderer* renderer) {
        TextureOption opt(image_data->width, image_data->height, TextureSourceType::DATA);
        opt.SetData(image_data->data);
        
        auto ret = std::make_shared<MarsTexture>(renderer, opt);
        return ret;
    }
};

}

#endif /* MN_RENDER_MATERIAL_H_ */
