//
//  Sampler.h
//  JustRay
//
//  Created by xin on 2017/6/24.
//
//

#pragma once
#include "Platform.h"
namespace JustRay
{
class Texture2DSampler
{
public:
    Texture2DSampler();
    ~Texture2DSampler();
    void UseDefaultSampler(int unit);
    void UseRepeatSampler(int unit);
    void UseLinearRepeatSampler(int unit);
    void UseLinearMirrorRepeatSampler(int unit);
    void UsePointSampler(int unit);
    void UsePointRepeatSampler(int unit);
    void UsePointMirrorRepeatSampler(int unit);
private:
    GLuint defaultSamplerID_;
    GLuint repeatSamplerID_;
    GLuint linearRepeatSamplerID_;
    GLuint linearMirrorRepeatSamplerID_;
    GLuint pointSamplerID_;
    GLuint pointRepeatSamplerID_;
    GLuint pointMirrorRepeatSamplerID_;
};
}

