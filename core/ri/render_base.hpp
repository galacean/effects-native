//
//  render_base.hpp
//
//  Created by Zongming Liu on 2022/4/7.
//

#ifndef render_base_hpp
#define render_base_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "ri/gl.h"
#include "math/vec2.hpp"
#include "math/vec3.hpp"
#include "math/vec4.hpp"
#include "math/mat3.hpp"
#include "math/mat4.hpp"

#include "sceneData/mars_data_base.h"

namespace mn {

class MarsTexture;

enum BufferDataType {
    BYTE = GL_BYTE,
    UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
    SHORT = GL_SHORT,
    UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
    B_INT = GL_INT,
    UNSIGNED_INT = GL_UNSIGNED_INT,
    FLOAT = GL_FLOAT,
};

enum UniformDataType {
    INT,
    INT_VEC2,
    INT_VEC3,
    INT_VEC4,
    FLOAT_VEC, // c++ enum语法 ugly;
    FLOAT_VEC2,
    FLOAT_VEC3,
    FLOAT_VEC4,
    FLOAT_MAT2,
    FLOAT_MAT3,
    FLOAT_MAT4,
    SAMPLER_2D,
    SAMPLER_CUBE
};

struct Shader {

    Shader() {}
    
    Shader(const std::string& frag, const std::string& vert, const std::string& n) : fragment(frag), vertex(vert), name(n)
    {
        
    }
    
    std::string fragment = "";
    
    std::string vertex = "";
    
    bool shaderd = false;
    
    std::string cache_id = "";
    
    // 和前端数据结构不一致
    std::vector<std::string> macros;
    
    std::string name = "";

};

struct RENDER_STATE_STENCIL_OP {
    uint32_t fail;
    uint32_t zfail;
    uint32_t zpass;
};

struct RENDER_STATE_STENCIL_FUNC {
    uint32_t func;
    int ref;
    uint32_t mask;
};

// todo: 优化
struct MaterialRenderStates {
    
    MaterialRenderStates() {
        
    }
    
    std::shared_ptr<MBool> sample_alpha_to_coverage;
    
    // blending;
    std::shared_ptr<MBool> blending;
    
    std::shared_ptr<MUint> blend_equation_rgb;
    
    std::shared_ptr<MUint> blend_equation_alpha;
    
    std::shared_ptr<MUint> blend_dst_alpha;
    
    std::shared_ptr<MUint> blend_src_alpha;
    
    std::shared_ptr<MUint> blend_src;
    
    std::shared_ptr<MUint> blend_dst;
    
    std::shared_ptr<Vec4> blend_color;
    
    std::shared_ptr<Vec4> color_mask;
    
    std::shared_ptr<MBool> depth_test;
    
    std::shared_ptr<MBool> depth_mask;
    
    std::shared_ptr<MFloat> depth_func;
    
    std::shared_ptr<MFloat> depth_range_znear;
    
    std::shared_ptr<MFloat> depth_range_zfar;
    
    std::shared_ptr<MUint> cull_face;
    
    std::shared_ptr<MBool> cull_face_enabled;
    
    std::shared_ptr<MUint> front_face = 0;
    
    // stencil
    std::shared_ptr<MBool> stencil_test;
    
    std::shared_ptr<MUint> stencil_mask;
    
    std::shared_ptr<MUint> stencil_mask_back;
    
    std::shared_ptr<MUint> stencil_mask_front;
    
    std::shared_ptr<RENDER_STATE_STENCIL_FUNC> stencil_func;
    
    std::shared_ptr<RENDER_STATE_STENCIL_FUNC> stencil_func_back;
    
    std::shared_ptr<RENDER_STATE_STENCIL_FUNC> stencil_func_front;
    
    std::shared_ptr<RENDER_STATE_STENCIL_OP> stencil_op;
    
    std::shared_ptr<RENDER_STATE_STENCIL_OP> stencil_op_back;
    
    std::shared_ptr<RENDER_STATE_STENCIL_OP> stencil_op_front;
    
    void AssignState(const MaterialRenderStates& state) {
        if (state.sample_alpha_to_coverage) {
            sample_alpha_to_coverage = state.sample_alpha_to_coverage;
        }
        if (state.blending) {
            blending = state.blending;
        }
        if (state.blend_equation_rgb) {
            blend_equation_rgb = state.blend_equation_rgb;
        }
        if (state.blend_equation_alpha) {
            blend_equation_alpha = state.blend_equation_alpha;
        }
        if (state.blend_dst_alpha) {
            blend_dst_alpha = state.blend_dst_alpha;
        }
        if (state.blend_src_alpha) {
            blend_src_alpha = state.blend_src_alpha;
        }
        if (state.blend_src) {
            blend_src = state.blend_src;
        }
        if (state.blend_dst) {
            blend_dst = state.blend_dst;
        }
        if (state.blend_color) {
            blend_color = state.blend_color;
        }
        if (state.color_mask) {
            color_mask = state.color_mask;
        }
        if (state.depth_test) {
            depth_test = state.depth_test;
        }
        if (state.depth_mask) {
            depth_mask = state.depth_mask;
        }
        if (state.depth_func) {
            depth_func = state.depth_func;
        }
        if (state.depth_range_znear) {
            depth_range_znear = state.depth_range_znear;
        }
        if (state.depth_range_zfar) {
            depth_range_zfar = state.depth_range_zfar;
        }
        if (state.cull_face) {
            cull_face = state.cull_face;
        }
        if (state.cull_face_enabled) {
            cull_face_enabled = state.cull_face_enabled;
        }
        if (state.front_face) {
            front_face = state.front_face;
        }
        if (state.stencil_test) {
            stencil_test = state.stencil_test;
        }
        if (state.stencil_mask) {
            stencil_mask = state.stencil_mask;
        }
        if (state.stencil_mask_back) {
            stencil_mask_back = state.stencil_mask_back;
        }
        if (state.stencil_mask_front) {
            stencil_mask_front = state.stencil_mask_front;
        }
        if (state.stencil_func) {
            stencil_func = state.stencil_func;
        }
        if (state.stencil_func_back) {
            stencil_func_back = state.stencil_func_back;
        }
        if (state.stencil_mask_front) {
            stencil_mask_front = state.stencil_mask_front;
        }
        if (state.stencil_op) {
            stencil_op = state.stencil_op;
        }
        if (state.stencil_op_back) {
            stencil_op_back = state.stencil_op_back;
        }
        if (state.stencil_op_front) {
            stencil_op_front = state.stencil_op_front;
        }
    }
};

// todo: 10.2.90 优化掉这个参数；没啥用，透传；
struct MaterialOptions {
    
    MaterialOptions(std::shared_ptr<MaterialRenderStates> st) : states(st) {
    }
    
    void SetSemanticValue(const std::string& key, const std::string& value) {
        uniform_semantics_.insert({ key, value });
    }
            
    Shader shader;
    
    std::string name;
    
    std::string shader_cached_id;
    
    std::unordered_map<std::string, std::string> uniform_semantics_;
    
    // todo: 先用shared_ptr，强制外部传入，稳定后改成对象
    std::shared_ptr<MaterialRenderStates> states;
    
};

// todo: explict;
struct UniformValue {
    
    UniformValue(UniformDataType type);
    
    UniformValue(UniformDataType type, bool semantic, const std::string& name);
    
    // uTex[a]; size代表uniform array的长度；
    UniformValue(UniformDataType type, size_t size);
            
    ~UniformValue();
        
    void SetData(const mn::Vec2& vec2, size_t index);
    
    void SetData(const mn::Vec3& vec3, size_t index);
    
    void SetData(const mn::Vec4& vec4, size_t index);
        
    void SetData(const mn::Mat3& mat3);
    
    void SetData(const mn::Mat4& mat4);
    
    void SetData(float *data, size_t index, size_t length);
        
    void SetData(float data, size_t index);
    
    void SetData(int data);
    
    void SetData(uint8_t*data, size_t byte_len);
    
    void ResetData(void* data);
    
    uint8_t* GetData() const {
        return data;
    }
    

    void SetTexture(std::shared_ptr<MarsTexture> tex);
        
    size_t byte_length = 0;
    
    UniformDataType data_type;
        
    std::shared_ptr<MarsTexture> texture;
    
    bool is_semantic = false;

    std::string semantic_name;
    
private:
    
    uint8_t* data = nullptr;
};

enum TextureSourceType {
    NONE,
    EMPTY,
    DATA,
    COMPRESSED,
    FRAMEBUFFER,
    MIPMAPS
};

struct TextureRawData {
    
    TextureRawData() {}
    
    TextureRawData(int w, int h) {
        width = w;
        height = h;
    }
    
    TextureRawData(int w, int h, uint8_t* d) {
        width = w;
        height = h;
        data = d;
    }
    
    int width = 0;
    
    int height = 0;
    
    uint8_t* data = nullptr;
    
    std::vector<uint8_t *> cube_data;
    
};

struct TextureMipmapRawData {
    
    TextureMipmapRawData() {}
    
    ~TextureMipmapRawData() {}
    
    std::vector<TextureRawData> mipmaps;
    
};

struct GLTextureInfo {
    
    GLTextureInfo() { }
    
    GLTextureInfo(GLenum t) : target(t) { }
    
    void SetEmptyInfo() {
        format = GL_LUMINANCE;
        internal_format = GL_LUMINANCE;
    }
    
    GLenum target = GL_TEXTURE_2D;
    
    GLenum format = GL_RGBA;
    
    GLenum internal_format = GL_RGBA;
    
    GLenum type = GL_UNSIGNED_BYTE;
    
    GLenum wrap_s = GL_CLAMP_TO_EDGE;
    
    GLenum wrap_t = GL_CLAMP_TO_EDGE;
    
    GLenum min_filter = GL_LINEAR;
    
    GLenum mag_filter = GL_LINEAR;
    
    uint32_t data_offset = 0;
    
    int byte_length = 0;
    
};

struct TextureOption {
    
    TextureOption(int w, int h, TextureSourceType _source_type) :  source_type(_source_type) {
        if (_source_type == TextureSourceType::EMPTY) {
            texture_info.SetEmptyInfo();
        }
        
        raw_data.width = w;
        raw_data.height = h;
    }
    
    TextureOption(TextureSourceType _source_type = TextureSourceType::NONE) : source_type(_source_type) {
        
    }
     
    ~TextureOption() {
        
    }
    
    // 不需要拷贝，外部直接用来创建texture，然后立即删除这个option和源数据
    void SetData(uint8_t *d) {
        raw_data.data = d;
    }
    
    TextureRawData raw_data;
    
    TextureMipmapRawData raw_mipmap_data;
                
    GLTextureInfo texture_info;
    
    TextureSourceType source_type;

    bool premultiply_alpha = false;

};

struct AttributeBase {
    
    AttributeBase() { }
    
    AttributeBase(uint32_t p_size, uint32_t p_stride, uint32_t p_offset) {
        size = p_size;
        stride = p_stride;
        offset = p_offset;
    }
    
    AttributeBase(uint32_t p_size, uint32_t p_stride, uint32_t p_offset, const std::string& source) {
        size = p_size;
        stride = p_stride;
        offset = p_offset;
        data_source = source;
    }
    
    AttributeBase(uint32_t p_size, uint32_t p_stride, uint32_t p_offset, uint32_t p_type) {
        size = p_size;
        stride = p_stride;
        offset = p_offset;
        type = p_type;
    }
    
    // size：向量的大小，vec4｜vec3｜vec2的大小分别为 4｜3｜2
    uint32_t size = 0;

    // specifying the offset in bytes between the beginning of consecutive vertex attributes, for pointer
    uint32_t stride = 0;
    
    // uint8_t == GLBoolean
    uint8_t normalize = false;
    
    // specifying an offset in bytes of the first component in the vertex attribute array
    uint32_t offset = 0;
    
    uint32_t instance_divisor = 0;
    
    // data_source：interleaved attribute, 标志共享字段
    std::string data_source = "";
    
    // GLenum meas: Float, Int, Short
    uint32_t type = GL_FLOAT;

};

struct AttributeWithData {
    
    AttributeWithData() {
        
    }
    
    AttributeWithData(uint32_t size, uint32_t stride, uint32_t offset)
                    : attribute_base(size, stride, offset) {
        
    }
    
    AttributeWithData(uint32_t size, uint32_t stride, uint32_t offset, uint32_t type)
                    : attribute_base(size, stride, offset, type) {
        
    }
    
    AttributeWithData(uint32_t size, uint32_t stride, uint32_t offset, const std::string& source)
                    : attribute_base(size, stride, offset, source) {
        
    }
    
    AttributeWithData(uint32_t size, uint32_t stride, uint32_t offset, uint32_t type, const std::string& source)
                    : attribute_base(size, stride, offset, type) {
        attribute_base.data_source = source;
    }
    
    ~AttributeWithData() {

    }
    
    void SetData(uint8_t *d, size_t byte_len) {
        data = d;
        byte_length = byte_len;
    }
    
    AttributeBase attribute_base;
    
    size_t byte_length = 0;
    
    // 外部float a[];
    uint8_t *data = nullptr;
};


// todo: 10.2.90 GPUCapability判断是否支持对应type;
enum RenderPassAttachmentStorageType {
    NONE_TYPE,
    // color_render_buffer;
    COLOR,
    // stencil_8_renderBuffer
    STENCIL_8_OPAQUE,
    // depth_16_renderbuffer
    DEPTH_16_OPAQUE,
    // depth_16 & stencil_8 render_buffer;
    DEPTH_STENCIL_OPAQUE,
    // depth_16 texture, need gpu capability
    DEPTH_16_TEXTURE,
    // depth_24&stencil_8, need gpu capability
    DEPTH_24_STENCIL_8_TEXTURE
};

struct RenderPassColorAttachmentOptions {
    
    RenderPassColorAttachmentOptions() {
        
    }
    
    RenderPassColorAttachmentOptions(std::shared_ptr<MarsTexture> _texture) {
        texture = _texture;
    }
    
    RenderPassColorAttachmentOptions(const TextureOption& _texture_options) {
        texture_option = _texture_options;
        use_texture_option = true;
    }
    
    ~RenderPassColorAttachmentOptions () {
        
    }
        
    std::shared_ptr<MarsTexture> texture;
    
    TextureOption texture_option;
    
    bool use_texture_option = false;
    
    int multi_sample = 0;
    
};

enum CLEAR_ACTION_TYPE {
    CLEAR_DEFAULT,
    CLEAR_COLOR,
    CLEAR_COLOR_DEPTH,
    CLEAR_COLOR_DEPTH_STENCIL
};

// todo: 10.2.90 default depth/ stencil value;
struct RenderPassClearAction {
    
    RenderPassClearAction() {
    
    }
    
    RenderPassClearAction(CLEAR_ACTION_TYPE action_type) {
        if (action_type == CLEAR_ACTION_TYPE::CLEAR_COLOR) {
            color_action = true;
            clear_color[0] = 1;
            clear_color[1] = 1;
            clear_color[2] = 1;
            clear_color[3] = 1;
        } else if (action_type == CLEAR_ACTION_TYPE::CLEAR_COLOR_DEPTH) {
            depth_action = true;
        } else if (action_type == CLEAR_ACTION_TYPE::CLEAR_COLOR_DEPTH_STENCIL) {
            depth_action = true;
            stencil_action = true;
        } else if (action_type == CLEAR_ACTION_TYPE::CLEAR_DEFAULT) {
            color_action = true;
            clear_color[0] = 1;
            clear_color[1] = 1;
            clear_color[2] = 1;
            clear_color[3] = 1;
            depth_action = true;
            clear_depth = 1.0;
            stencil_action = true;
            clear_stencil = 0.0;
        }
    }
    
    float clear_color[4] = { 0 } ;
    
    float clear_depth = 0;
    
    uint32_t clear_stencil = 0;
    
    bool color_action = false;
    
    bool depth_action = false;
    
    bool stencil_action = false;
    
};

enum TextureStoreAction {
    store = 0,
    clear = 2
};

struct RenderPassStoreAction {
    
    TextureStoreAction depth_aciton;
    
    TextureStoreAction stencil_action;
    
    TextureStoreAction color_action;

};


enum RenderPassMeshOrder {
    ASCENDING,
    DESCENDING
};


struct RenderPassAttachmentOptions {
    
    RenderPassAttachmentOptions() {
        
    }
    
    RenderPassAttachmentOptions(int x, int y, int w, int h,  float _viewport_scale = 1): viewport_scale(_viewport_scale) {
        is_custom_viewport = true;
        viewport[0] = x;
        viewport[1] = y;
        viewport[2] = w;
        viewport[3] = h;
    }
    
    void SetViewport(int x, int y, int w, int h) {
        is_custom_viewport = true;
        viewport[0] = x;
        viewport[1] = y;
        viewport[2] = w;
        viewport[3] = h;
    }
    
    void AddRenderPassColorAttachmentOption(const RenderPassColorAttachmentOptions& color_attachmment_option) {
        color_attachment_options.push_back(color_attachmment_option);
    }
    
    void AddRenderPassColorAttachmentOption(std::shared_ptr<MarsTexture> texture) {
        RenderPassColorAttachmentOptions color_attachment_option(texture);
        color_attachment_options.push_back(color_attachment_option);
    }
    
    void SetDepthStencilAttachmentType(RenderPassAttachmentStorageType _storage_type) {
        depth_stencil_storage_type = _storage_type;
    }
        
    std::vector<RenderPassColorAttachmentOptions> color_attachment_options;
    
    RenderPassAttachmentStorageType depth_stencil_storage_type = RenderPassAttachmentStorageType::NONE_TYPE;
                
    float viewport_scale = 1;
    
    bool is_custom_viewport = false;   // true：表示使用viewport参数的值，false表示去renderer的宽高；
    
    int viewport[4] = { 0 };
    
};

}

#endif /* render_base_hpp */
