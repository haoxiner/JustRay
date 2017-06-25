//
//  RenderEngine.hpp
//  JustRay
//
//  Created by xin on 2017/6/17.
//
//
#pragma once
#include "GBuffer.h"
#include "ShaderProgram.h"
#include "ModelGroup.h"
#include "Material.h"
#include "Cubemap.h"
#include "Texture2DSampler.h"
#include "SSAO.h"
#include "MathUtil.h"
#include "Platform.h"
#include <vector>
#include <memory>
#include <tuple>
namespace JustRay
{
class RenderEngine
{
public:
    void Startup(int xResolution, int yResolution);
    void SetEnvironment(const std::string& name);
    void SetCamera(const Float3& position, const Float3& focus, const Float3& up);
    void SubmitToScreen();
    void Render(const ModelGroup& modelGroup);
    void Prepare();
    void Submit(const ModelGroup& modelGroup);
private:
    void SetupShader();
    void SetupConstantBuffers();
    void SetupPreIntegratedData();
    void SetupGBuffer();
    int xResolution_;
    int yResolution_;
private:
    std::unique_ptr<Cubemap> specularCubemap_;
    std::unique_ptr<Cubemap> diffuseCubemap_;
    std::unique_ptr<GBuffer> gBuffer_;
    std::unique_ptr<Texture2DSampler> texture2DSampler_;
//    std::unique_ptr<SSAO> ssao_;
private:
    GLuint gBufferShader_;
    GLuint pbrShader_;
    GLuint pbrShaderForStationaryEntity_;
private:
    GLuint squareVertexArrayID_;
    GLuint squareVertexBufferID_;
    GLuint screenFrameBufferID_;
private:
    GLuint dfgTextureID_;
    std::vector<GLuint> bufferList_;
};
}
