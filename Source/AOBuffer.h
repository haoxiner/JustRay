//
//  AOBuffer.h
//  JustRay
//
//  Created by xin on 2017/6/26.
//
//

#pragma once
#include "Texture2DSampler.h"
namespace JustRay
{
class AOBuffer
{
public:
    AOBuffer(int xResolution, int yResolution);
    ~AOBuffer();
    void CalculateOcclusion(Texture2DSampler &texture2DSampler, GLuint quadVertexArrayID, GLuint normalBufferID, GLuint depthBufferID);
    void UseOcclusionBufferAsTexture(int unit);
    void UseBluredOcclusionBufferAsTexture(int unit);
private:
    GLuint occlusionShader_;
    GLuint occlusionFBO_;
    GLuint occlusionBuffer_;
    
    GLuint noiseBuffer_;
    
    GLuint uniformBufferID_;
    
    int screenWidth_;
    int screenHeight_;
    int occlusionWidth_;
    int occlusionHeight_;
};
}

