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
private:
    GLuint occlusionShader_;
    GLuint occlusionFBO_;
    GLuint occlusionBuffer_;
    
    GLuint noiseBuffer_;
    GLuint uniformBufferID_;
    
    GLuint blurXFBO_;
    GLuint blurYFBO_;
    GLuint blurXShader_;
    GLuint blurYShader_;
    GLuint blurXBuffer_;
    GLuint blurYBuffer_;

    int screenWidth_;
    int screenHeight_;
    int occlusionWidth_;
    int occlusionHeight_;
};
}

