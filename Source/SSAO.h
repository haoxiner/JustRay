//
//  SSAO.h
//  JustRay
//
//  Created by xin on 2017/6/24.
//
//

#pragma once
#include "Texture2DSampler.h"
#include "Platform.h"
namespace JustRay
{
class SSAO
{
public:
    SSAO(int xResolution, int yResolution);
    ~SSAO();
    void CalculateOcclusion(Texture2DSampler &texture2DSampler, GLuint quadVertexArrayID, GLuint normalBufferID, GLuint depthBufferID);
    void UseOcclusionBufferAsTexture(int unit);
    void UseBluredOcclusionBufferAsTexture(int unit);
private:
    int width_ = 0;
    int height_ = 0;
private:
    GLuint noiseTextureID_;
    GLuint frameBufferID_;
    
    GLuint bluredFrameBufferID_;
    GLuint bluredOcclusionBufferID_;
    
    GLuint resultFrameBufferID_;
    GLuint resultOcclusionBufferID_;
    
    GLuint occlusionBufferID_;
    GLuint occlusionShader_;
    GLuint blurShader_;
    GLuint uniformBufferID_;
};
}

