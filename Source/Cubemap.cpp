//
//  Cubemap.cpp
//  JustRay
//
//  Created by xin on 2017/6/19.
//
//

#include "Cubemap.h"
#include "AppleSystemTools.h"
#include "half.h"
#include <fstream>
#include <vector>
#include <iostream>
namespace JustRay
{
Cubemap::Cubemap(const std::string& name)
{
    glGenTextures(1, &textureID_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID_);
    std::ifstream file(GetFilePath(name), std::ios::binary);
    short header[4];
    file.read(reinterpret_cast<char*>(header), sizeof(header));
    width_ = header[0];
    height_ = header[1];
    maxMipLevel_ = header[3];
    for (int level = 0, width = width_, height = height_; level <= maxMipLevel_; level++) {
        if (level == 0) {
            glTexStorage2D(GL_TEXTURE_CUBE_MAP, maxMipLevel_ + 1, GL_RGB16F, width, height);
        }
        std::vector<half> halfData(width * height * header[2]);
        std::vector<float> floatData(width * height * header[2]);
        for (GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X; face < GL_TEXTURE_CUBE_MAP_POSITIVE_X + 6; face++) {
            file.read(reinterpret_cast<char*>(halfData.data()), halfData.size()*sizeof(half));
            for (int i = 0; i < floatData.size(); i++) {
                floatData[i] = (float)halfData[i];
            }
            glTexSubImage2D(face, level, 0, 0, width, height, GL_RGB, GL_FLOAT, floatData.data());
        }
        width /= 2;
        height /= 2;
    }
    file.close();
    glGenSamplers(1, &samplerID_);
    glSamplerParameteri(samplerID_, GL_TEXTURE_MIN_FILTER, maxMipLevel_ > 0 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glSamplerParameteri(samplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(samplerID_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(samplerID_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(samplerID_, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(samplerID_, GL_TEXTURE_MAX_LOD, maxMipLevel_);
    glSamplerParameteri(samplerID_, GL_TEXTURE_MIN_LOD, 0);
}
Cubemap::~Cubemap()
{
    glDeleteSamplers(1, &samplerID_);
    glDeleteTextures(1, &textureID_);
}
void Cubemap::Bind(int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID_);
    glBindSampler(unit, samplerID_);
}
};
