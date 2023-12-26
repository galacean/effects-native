//
//  gl.h
//
//  Created by Zongming Liu on 2021/11/10.
//

#ifndef gl_h
#define gl_h

#ifdef ANDROID
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#else
#include <OpenGLES/gltypes.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif

void CheckGLError(const char* stmt, const char* fname, int line);

#ifdef DEBUG
    #define GL_CHECK(stmt) do { \
            CheckGLError("pre error", __FILE__, __LINE__); \
            stmt; \
            CheckGLError(#stmt, __FILE__, __LINE__); \
        } while (0)
#else
    #define GL_CHECK(stmt) stmt
#endif

#endif /* gl_h */
