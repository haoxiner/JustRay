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
#include "AOBuffer.h"
#include "SkyBox.h"
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
    void SetEffect(float ao, float exposure);
    void SubmitToScreen();
    void Prepare();
    void Submit(const ModelGroup& modelGroup);
    void Submit(const ModelGroup& modelGroup, Material& material);
    int GetXResolution() {return xResolution_;}
    int GetYResolution() {return yResolution_;}
private:
    struct PerObjectBuffer
    {
        Matrix4x4 modelToWorld;
        Float4 material[2];
    }perObjectBuffer_;
    struct PerFrameBuffer
    {
        Float4 perFrameData;
        Float4 cameraPosition;
        Matrix4x4 worldToView;
    }perFrameBuffer_;
    struct PerEngineBuffer
    {
        Matrix4x4 viewToProjection;
    }perEngineBuffer_;
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
    std::unique_ptr<AOBuffer> aoBuffer_;
    std::unique_ptr<SkyBox> skyBox_;
private:
    GLuint gBufferShader_;
    GLuint pbrShader_;
    GLuint skyShader_;
private:
    GLuint squareVertexArrayID_;
    GLuint squareVertexBufferID_;
    GLuint screenFrameBufferID_;
private:
    GLuint dfgTextureID_;
    std::vector<GLuint> bufferList_;
};
}
