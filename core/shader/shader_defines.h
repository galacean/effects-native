#ifndef MN_SHADER_SHADER_DEFINES_H_
#define MN_SHADER_SHADER_DEFINES_H_

#ifdef ANDROID
const std::string SHADER_PRECISION = "\nprecision highp float;\n";
#else
const std::string SHADER_PRECISION = "\nprecision mediump float;\n";
#endif

const std::string SHADER_PRECISION_HIGHP = "\nprecision highp float;\n";

#endif
