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
    
    glGenSamplers(1, &pointSamplerID_);
    glSamplerParameteri(pointSamplerID_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(pointSamplerID_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(pointSamplerID_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(pointSamplerID_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
Texture2DSampler::~Texture2DSampler()
{
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
void Texture2DSampler::UsePointSampler(int unit)
{
    glBindSampler(unit, pointSamplerID_);
}
};
