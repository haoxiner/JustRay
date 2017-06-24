//
//  DeferredRenderer.h
//  JustRay
//
//  Created by xin on 2017/6/24.
//
//

#pragma once
#include "ShaderProgram.h"
#include "ModelGroup.h"
#include "Material.h"
#include "Cubemap.h"
#include "MathUtil.h"
#include "Platform.h"
#include <vector>
#include <memory>
#include <tuple>
namespace JustRay
{
class GBuffer
{
public:
    GBuffer(int xResolution, int yResolution);
    ~GBuffer();
    void UseAsRenderTarget();
    void UseAsTextures(int unitOffset);
    void UseNormalBufferAsTexture(int unit);
    void UseDepthBufferAsTexture(int unit);
private:
    GLuint deferredFrameBufferID_;
    GLuint gBuffer0_;
    GLuint gBuffer1_;
    GLuint gBuffer2_;
    GLuint depthBufferID_;
};
}

