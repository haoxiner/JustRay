//
//  Sampler.cpp
//  JustRay
//
//  Created by xin on 2017/6/24.
//
//

#include "Texture2DSampler.h"
namespace JustRay
{
Texture2DSampler::Texture2DSampler()
{
    glGenSamplers(1, &defaultSamplerID_);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glGenSamplers(1, &repeatSamplerID_);
    glSamplerParameteri(repeatSamplerID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(repeatSamplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(repeatSamplerID_, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(repeatSamplerID_, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(repeatSamplerID_, GL_TEXTURE_MIN_LOD, 0);
    glSamplerParameteri(repeatSamplerID_, GL_TEXTURE_MAX_LOD, 4);
    float aniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
    glSamplerParameterf(repeatSamplerID_, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
    
    glGenSamplers(1, &linearRepeatSamplerID_);
    glSamplerParameteri(linearRepeatSamplerID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(linearRepeatSamplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(linearRepeatSamplerID_, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(linearRepeatSamplerID_, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glGenSamplers(1, &linearMirrorRepeatSamplerID_);
    glSamplerParameteri(linearMirrorRepeatSamplerID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(linearMirrorRepeatSamplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(linearMirrorRepeatSamplerID_, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glSamplerParameteri(linearMirrorRepeatSamplerID_, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    
    glGenSamplers(1, &pointSamplerID_);
    glSamplerParameteri(pointSamplerID_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(pointSamplerID_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(pointSamplerID_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(pointSamplerID_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glGenSamplers(1, &pointRepeatSamplerID_);
    glSamplerParameteri(pointRepeatSamplerID_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(pointRepeatSamplerID_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(pointRepeatSamplerID_, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(pointRepeatSamplerID_, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glGenSamplers(1, &pointMirrorRepeatSamplerID_);
    glSamplerParameteri(pointMirrorRepeatSamplerID_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(pointMirrorRepeatSamplerID_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(pointMirrorRepeatSamplerID_, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glSamplerParameteri(pointMirrorRepeatSamplerID_, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
}
Texture2DSampler::~Texture2DSampler()
{
    glDeleteSamplers(1, &pointRepeatSamplerID_);
    glDeleteSamplers(1, &pointSamplerID_);
    glDeleteSamplers(1, &repeatSamplerID_);
    glDeleteSamplers(1, &defaultSamplerID_);
}
void Texture2DSampler::UseDefaultSampler(int unit)
{
    glBindSampler(unit, defaultSamplerID_);
}
void Texture2DSampler::UseRepeatSampler(int unit)
{
    glBindSampler(unit, repeatSamplerID_);
}
void Texture2DSampler::UseLinearRepeatSampler(int unit)
{
    glBindSampler(unit, linearRepeatSamplerID_);
}
void Texture2DSampler::UseLinearMirrorRepeatSampler(int unit)
{
    glBindSampler(unit, linearMirrorRepeatSamplerID_);
}
void Texture2DSampler::UsePointSampler(int unit)
{
    glBindSampler(unit, pointSamplerID_);
}
void Texture2DSampler::UsePointRepeatSampler(int unit)
{
    glBindSampler(unit, pointRepeatSamplerID_);
}
void Texture2DSampler::UsePointMirrorRepeatSampler(int unit)
{
    glBindSampler(unit, pointMirrorRepeatSamplerID_);
}
};
