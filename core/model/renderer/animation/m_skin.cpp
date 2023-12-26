//
//  m_skin.cpp
//
//  Created by Zongming Liu on 2022/10/12.
//

#include "m_skin.hpp"
#include "math/math_util.hpp"
#include "ri/render/mars_renderer.hpp"

namespace mn {

MSkin::MSkin(MarsRenderer* renderer) : renderer_(renderer) {
    
}

MSkin::~MSkin() {
    
}

void MSkin::Create(std::shared_ptr<MarsSkinOptions> options, std::shared_ptr<TreeVFXItem> parent_item) {
    parent_item_ = parent_item;
    joint_list_ = options->joints;
    
    if (options->inverse_bind_matrices != nullptr && options->matrices_byte_length > 0) {
        if (options->matrices_byte_length % 16 != 0 || options->matrices_byte_length != joint_list_.size() * (16 * sizeof(float))) {
            MLOGE("MSkin Create Failed inverse matrices not match");
            assert(false);
        }
        
        size_t matrix_count = options->matrices_byte_length / (16 * sizeof(float));
        uint8_t* matrices_data = options->inverse_bind_matrices;
        for (size_t i=0; i<matrix_count; i++) {
            Mat4 mat;
            uint8_t* data = matrices_data + (i * 16 * (sizeof(float)));
            mat.Unpack(data);
            DEBUG_DATA("MSin_Inverse_Bind_Matrices", data, 16 * sizeof(float), sizeof(float));
            inverse_bind_matrices_.push_back(mat);
        }
    }

}

void MSkin::UpdateSkinMatrices() {
    this->animation_matrices_.clear();
    if (std::shared_ptr<TreeVFXItem> parent_item = parent_item_.lock()) {
        auto parent_tree = (Tree*) parent_item->GetContent();
        if (!parent_tree) {
            // todo: 临时规避闪退
            MLOGE("!!!\n临时规避闪退\n!!!");
            return;
        }
        for (size_t i=0; i<joint_list_.size(); i++) {
            auto node = parent_tree->GetNodeById(std::to_string(joint_list_[i]));
            if (node) {
                Mat4 node_world_matrix;
                node->transform->GetWorldMatrix(node_world_matrix);
                this->animation_matrices_.push_back(node_world_matrix);
            }
        }
        
        if (this->animation_matrices_.size() == this->inverse_bind_matrices_.size()) {
            for (size_t i=0; i<animation_matrices_.size(); i++) {
                MathUtil::Mat4Multiply(animation_matrices_[i], animation_matrices_[i], inverse_bind_matrices_[i]);
            }
        } else {
            DEBUG_MLOGD("animation matrix size not match inverse bind matrices");
            this->animation_matrices_.clear();
            for (size_t i=0; i<inverse_bind_matrices_.size(); i++) {
                animation_matrices_.push_back(inverse_bind_matrices_[i]);
            }
        }
    }
}

void MSkin::UpdateParentItem(std::shared_ptr<TreeVFXItem> parent_item) {
    parent_item_ = parent_item;
}

bool MSkin::IsTextureMode() {
    std::shared_ptr<GLGPUCapability> capability = renderer_->GPUCapability();

    int required_for_most_feature = 25;
    int avaliable_max_vertex_uniforms = capability->GetMaxVertexUniforms() - required_for_most_feature;
    int uniform_required_for_most_features = 8;

    if (force_skin_texture_ || JointCount() > (avaliable_max_vertex_uniforms / uniform_required_for_most_features)){
        // todo: texture_float;
//        if (capability->SupportHalfFloatTexture()) {
//            return true;
//        } else {
//            return false;
//        }
        // todo: gl_es3;
        return true;
    }
    
    return false;
}

}
