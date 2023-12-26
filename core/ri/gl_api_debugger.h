#ifndef MN_GL_API_DEBUGGER_H_
#define MN_GL_API_DEBUGGER_H_

#ifdef BUILD_PLAYGROUND
#define INJECT_DEBUG_OBJECT 1
#endif

#ifdef INJECT_DEBUG_OBJECT

#include <stdint.h>
#include <string>

class GLDebugObj {
public:
    virtual void DrawElements(uint32_t mode, int32_t count, uint32_t type, const void* offset) {}
    virtual void DrawArrays(uint32_t mode, int32_t first, int32_t count) {}
    virtual void CreateProgram(uint32_t program) {}
    virtual void UseProgram(uint32_t program) {}
    virtual void GetProgramParameter(uint32_t program, uint32_t pname, int32_t* params) {}
    virtual void GetActiveAttrib(uint32_t program, uint32_t index, int32_t size, uint32_t type, const char* name) {}
    virtual void GetAttribLocation(uint32_t program, const char* name, int32_t location) {}
    virtual void VertexAttribPointer(uint32_t location, int32_t size, uint32_t type, uint8_t normalized, int32_t stride, const void* offset) {}
    virtual void EnableVertexAttribArray(uint32_t index) {}
    virtual void DisableVertexAttribArray(uint32_t index) {}
    virtual void GetActiveUniform(uint32_t program, uint32_t index, int32_t size, uint32_t type, const char* name) {}
    virtual void GetUniformLocation(uint32_t program, const char* name, int32_t location) {}
    virtual void Uniform1f(int32_t location, float v0) {}
    virtual void Uniform1fv(int32_t location, int32_t count, const float *value) {}
    virtual void Uniform1i(int32_t location, int32_t v0) {}
    virtual void Uniform1iv(int32_t location, int32_t count, const int32_t *value) {}
    virtual void Uniform2f(int32_t location, float v0, float v1) {}
    virtual void Uniform2fv(int32_t location, int32_t count, const float *value) {}
    virtual void Uniform2i(int32_t location, int32_t v0, int32_t v1) {}
    virtual void Uniform2iv(int32_t location, int32_t count, const int32_t *value) {}
    virtual void Uniform3f(int32_t location, float v0, float v1, float v2) {}
    virtual void Uniform3fv(int32_t location, int32_t count, const float *value) {}
    virtual void Uniform3i(int32_t location, int32_t v0, int32_t v1, int32_t v2) {}
    virtual void Uniform3iv(int32_t location, int32_t count, const int32_t *value) {}
    virtual void Uniform4f(int32_t location, float v0, float v1, float v2, float v3) {}
    virtual void Uniform4fv(int32_t location, int32_t count, const float *value) {}
    virtual void Uniform4i(int32_t location, int32_t v0, int32_t v1, int32_t v2, int32_t v3) {}
    virtual void Uniform4iv(int32_t location, int32_t count, const int32_t *value) {}
    virtual void UniformMatrix2fv(int32_t location, int32_t count, uint8_t transpose, const float* value) {}
    virtual void UniformMatrix3fv(int32_t location, int32_t count, uint8_t transpose, const float* value) {}
    virtual void UniformMatrix4fv(int32_t location, int32_t count, uint8_t transpose, const float* value) {}
    virtual void CreateBuffer(uint32_t buffer) {}
    virtual void BindBuffer(uint32_t target, uint32_t buffer) {}
    virtual void BufferData(uint32_t target, size_t size, const void* data, uint32_t usage) {}
    virtual void BufferSubData(uint32_t target, intptr_t offset, size_t size, const void* data) {}
    virtual void AttachShader(uint32_t program, uint32_t shader) {}
    virtual void ShaderSource(uint32_t shader, int32_t count, const char *const *string) {}
    virtual void Enable(uint32_t cap) {}
    virtual void Disable(uint32_t cap) {}
    virtual void StencilFunc(uint32_t func, int32_t ref, uint32_t mask) {}
    virtual void StencilFuncSeparate(uint32_t face, uint32_t func, int32_t ref, uint32_t mask) {}
    virtual void StencilMask(uint32_t mask) {}
    virtual void StencilMaskSeparate(uint32_t face, uint32_t mask) {}
    virtual void StencilOp(uint32_t fail, uint32_t zfail, uint32_t zpass) {}
    virtual void StencilOpSeparate(uint32_t face, uint32_t sfail, uint32_t dpfail, uint32_t dppass) {}
};

#define DEBUG_DrawElements(mode, count, type, offset) {if (s_debug_obj_) s_debug_obj_->DrawElements(mode, count, type, offset);}
#define DEBUG_DrawArrays(mode, first, count) {if (s_debug_obj_) s_debug_obj_->DrawArrays(mode, first, count);}
#define DEBUG_CreateProgram(program) {if (s_debug_obj_) s_debug_obj_->CreateProgram(program);}
#define DEBUG_UseProgram(program) {if (s_debug_obj_) s_debug_obj_->UseProgram(program);}
#define DEBUG_GetProgramParameter(program, pname, params) {if (s_debug_obj_) s_debug_obj_->GetProgramParameter(program, pname, params);}
#define DEBUG_GetActiveAttrib(program, index, size, type, name) {if (s_debug_obj_) s_debug_obj_->GetActiveAttrib(program, index, size, type, name);}
#define DEBUG_GetAttribLocation(program, name, location) {if (s_debug_obj_) s_debug_obj_->GetAttribLocation(program, name, location);}
#define DEBUG_VertexAttribPointer(location, size, type, normalized, stride, offset) {if (s_debug_obj_) s_debug_obj_->VertexAttribPointer(location, size, type, normalized, stride, offset);}
#define DEBUG_EnableVertexAttribArray(index) {if (s_debug_obj_) s_debug_obj_->EnableVertexAttribArray(index);}
#define DEBUG_DisableVertexAttribArray(index) {if (s_debug_obj_) s_debug_obj_->DisableVertexAttribArray(index);}
#define DEBUG_GetActiveUniform(program, index, size, type, name) {if (s_debug_obj_) s_debug_obj_->GetActiveUniform(program, index, size, type, name);}
#define DEBUG_GetUniformLocation(program, name, location) {if (s_debug_obj_) s_debug_obj_->GetUniformLocation(program, name, location);}
#define DEBUG_Uniform1f(location, v0) {if (s_debug_obj_) s_debug_obj_->Uniform1f(location, v0);}
#define DEBUG_Uniform1fv(location, count, value) {if (s_debug_obj_) s_debug_obj_->Uniform1fv(location, count, value);}
#define DEBUG_Uniform1i(location, v0) {if (s_debug_obj_) s_debug_obj_->Uniform1i(location, v0);}
#define DEBUG_Uniform1iv(location, count, value) {if (s_debug_obj_) s_debug_obj_->Uniform1iv(location, count, value);}
#define DEBUG_Uniform2f(location, v0, v1) {if (s_debug_obj_) s_debug_obj_->Uniform2f(location, v0, v1);}
#define DEBUG_Uniform2fv(location, count, value) {if (s_debug_obj_) s_debug_obj_->Uniform2fv(location, count, value);}
#define DEBUG_Uniform2i(location, v0, v1) {if (s_debug_obj_) s_debug_obj_->Uniform2i(location, v0, v1);}
#define DEBUG_Uniform2iv(location, count, value) {if (s_debug_obj_) s_debug_obj_->Uniform2iv(location, count, value);}
#define DEBUG_Uniform3f(location, v0, v1, v2) {if (s_debug_obj_) s_debug_obj_->Uniform3f(location, v0, v1, v2);}
#define DEBUG_Uniform3fv(location, count, value) {if (s_debug_obj_) s_debug_obj_->Uniform3fv(location, count, value);}
#define DEBUG_Uniform3i(location, v0, v1, v2) {if (s_debug_obj_) s_debug_obj_->Uniform3i(location, v0, v1, v2);}
#define DEBUG_Uniform3iv(location, count, value) {if (s_debug_obj_) s_debug_obj_->Uniform3iv(location, count, value);}
#define DEBUG_Uniform4f(location, v0, v1, v2, v3) {if (s_debug_obj_) s_debug_obj_->Uniform4f(location, v0, v1, v2, v3);}
#define DEBUG_Uniform4fv(location, count, value) {if (s_debug_obj_) s_debug_obj_->Uniform4fv(location, count, value);}
#define DEBUG_Uniform4i(location, v0, v1, v2, v3) {if (s_debug_obj_) s_debug_obj_->Uniform4i(location, v0, v1, v2, v3);}
#define DEBUG_Uniform4iv(location, count, value) {if (s_debug_obj_) s_debug_obj_->Uniform4iv(location, count, value);}
#define DEBUG_UniformMatrix2fv(location, count, transpose, value) {if (s_debug_obj_) s_debug_obj_->UniformMatrix2fv(location, count, transpose, value);}
#define DEBUG_UniformMatrix3fv(location, count, transpose, value) {if (s_debug_obj_) s_debug_obj_->UniformMatrix3fv(location, count, transpose, value);}
#define DEBUG_UniformMatrix4fv(location, count, transpose, value) {if (s_debug_obj_) s_debug_obj_->UniformMatrix4fv(location, count, transpose, value);}
#define DEBUG_CreateBuffer(buffer) {if (s_debug_obj_) s_debug_obj_->CreateBuffer(buffer);}
#define DEBUG_BindBuffer(target, buffer) {if (s_debug_obj_) s_debug_obj_->BindBuffer(target, buffer);}
#define DEBUG_BufferData(target, size, data, usage) {if (s_debug_obj_) s_debug_obj_->BufferData(target, size, data, usage);}
#define DEBUG_BufferSubData(target, offset, size, data) {if (s_debug_obj_) s_debug_obj_->BufferSubData(target, offset, size, data);}
#define DEBUG_AttachShader(program, shader) {if (s_debug_obj_) s_debug_obj_->AttachShader(program, shader);}
#define DEBUG_ShaderSource(shader, count, string) {if (s_debug_obj_) s_debug_obj_->ShaderSource(shader, count, string);}
#define DEBUG_Enable(cap) {if (s_debug_obj_) s_debug_obj_->Enable(cap);}
#define DEBUG_Disable(cap) {if (s_debug_obj_) s_debug_obj_->Disable(cap);}
#define DEBUG_StencilFunc(func, ref, mask)  {if (s_debug_obj_) s_debug_obj_->StencilFunc(func, ref, mask);}
#define DEBUG_StencilFuncSeparate(face, func, ref, mask)  {if (s_debug_obj_) s_debug_obj_->StencilFuncSeparate(face, func, ref, mask);}
#define DEBUG_StencilMask(mask) {if (s_debug_obj_) s_debug_obj_->StencilMask(mask);}
#define DEBUG_StencilMaskSeparate(face, mask) {if (s_debug_obj_) s_debug_obj_->StencilMaskSeparate(face, mask);}
#define DEBUG_StencilOp(fail, zfail, zpass) {if (s_debug_obj_) s_debug_obj_->StencilOp(fail, zfail, zpass);}
#define DEBUG_StencilOpSeparate(face, sfail, dpfail, dppass) {if (s_debug_obj_) s_debug_obj_->StencilOpSeparate(face, sfail, dpfail, dppass);}

#else

#define DEBUG_DrawElements(mode, count, type, offset)
#define DEBUG_DrawArrays(mode, first, count)
#define DEBUG_CreateProgram(program)
#define DEBUG_UseProgram(program)
#define DEBUG_GetProgramParameter(program, pname, params)
#define DEBUG_GetActiveAttrib(program, index, size, type, name)
#define DEBUG_GetAttribLocation(program, name, location)
#define DEBUG_VertexAttribPointer(location, size, type, normalized, stride, offset)
#define DEBUG_EnableVertexAttribArray(index)
#define DEBUG_DisableVertexAttribArray(index)
#define DEBUG_GetActiveUniform(program, index, size, type, name)
#define DEBUG_GetUniformLocation(program, name, location)
#define DEBUG_Uniform1f(location, v0)
#define DEBUG_Uniform1fv(location, count, value)
#define DEBUG_Uniform1i(location, v0)
#define DEBUG_Uniform1iv(location, count, value)
#define DEBUG_Uniform2f(location, v0, v1)
#define DEBUG_Uniform2fv(location, count, value)
#define DEBUG_Uniform2i(location, v0, v1)
#define DEBUG_Uniform2iv(location, count, value)
#define DEBUG_Uniform3f(location, v0, v1, v2)
#define DEBUG_Uniform3fv(location, count, value)
#define DEBUG_Uniform3i(location, v0, v1, v2)
#define DEBUG_Uniform3iv(location, count, value)
#define DEBUG_Uniform4f(location, v0, v1, v2, v3)
#define DEBUG_Uniform4fv(location, count, value)
#define DEBUG_Uniform4i(location, v0, v1, v2, v3)
#define DEBUG_Uniform4iv(location, count, value)
#define DEBUG_UniformMatrix2fv(location, count, transpose, value)
#define DEBUG_UniformMatrix3fv(location, count, transpose, value)
#define DEBUG_UniformMatrix4fv(location, count, transpose, value)
#define DEBUG_CreateBuffer(buffer)
#define DEBUG_BindBuffer(target, buffer)
#define DEBUG_BufferData(target, size, data, usage)
#define DEBUG_BufferSubData(target, offset, size, data)
#define DEBUG_AttachShader(program, shader)
#define DEBUG_ShaderSource(shader, count, string)
#define DEBUG_Enable(cap)
#define DEBUG_Disable(cap)
#define DEBUG_StencilFunc(func, ref, mask)
#define DEBUG_StencilFuncSeparate(face, func, ref, mask)
#define DEBUG_StencilMask(mask)
#define DEBUG_StencilMaskSeparate(face, mask)
#define DEBUG_StencilOp(fail, zfail, zpass)
#define DEBUG_StencilOpSeparate(face, sfail, dpfail, dppass)

#endif

#endif
