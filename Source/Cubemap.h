//
//  Cubemap.h
//  JustRay
//
//  Created by xin on 2017/6/19.
//
//

#pragma once
#include "Platform.h"
#include <string>
namespace JustRay
{
class Cubemap
{
friend class RenderEngine;
public:
    Cubemap(const std::string& name);
    ~Cubemap();
    void Bind(int unit);
private:
    GLuint textureID_ = 0;
    GLuint samplerID_ = 0;
    int width_ = 0;
    int height_ = 0;
    int maxMipLevel_ = 0;
};
}

