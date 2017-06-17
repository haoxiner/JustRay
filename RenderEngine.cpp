//
//  RenderEngine.cpp
//  JustRay
//
//  Created by xin on 2017/6/17.
//
//

#include "RenderEngine.h"
#include <vector>
namespace JustRay
{
struct PerObjectBuffer
{
    Matrix4x4 modelToWorld;
    Float4 material;
};
struct PerFrameBuffer
{
    Float4 cameraPosition;
    Matrix4x4 worldToView;
};
struct PerEngineBuffer
{
    Matrix4x4 viewToProjection;
};
enum ConstantBufferType
{
    PER_ENGINE_BUFFER = 0,
    PER_FRAME_BUFFER,
    PER_OBJECT_BUFFER,
    NUM_OF_BUFFER
};
void RenderEngine::Startup(int xResolution, int yResolution)
{
    xResolution_ = xResolution;
    yResolution_ = yResolution;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glViewport(0, 0, xResolution, yResolution);

    glGenSamplers(1, &defaultSamplerID_);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glGenSamplers(1, &repeatSamplerID_);
    glSamplerParameteri(repeatSamplerID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(repeatSamplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(repeatSamplerID_, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(repeatSamplerID_, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindSampler(3, repeatSamplerID_);
    glBindSampler(4, repeatSamplerID_);
    glBindSampler(5, repeatSamplerID_);

    SetupConstantBuffers();
    SetupPreIntegratedData();
//    SetupEnvironment("uffizi");

//    stationaryEntityShader_.Startup(ResourceManager::ReadFileToString("./Shaders/Stationary.vert.glsl"), ResourceManager::ReadFileToString("./Shaders/PBR.frag.glsl"));
//    animatingEntityShader_.Startup(ResourceManager::ReadFileToString("./Shaders/PBR.vert.glsl"), ResourceManager::ReadFileToString("./Shaders/PBR.frag.glsl"));
//    skyBoxShader_.Startup(ResourceManager::ReadFileToString("./Shaders/SkyBox.vert.glsl"), ResourceManager::ReadFileToString("./Shaders/SkyBox.frag.glsl"));
}
void RenderEngine::SetupShader()
{
    
}
void RenderEngine::SetupConstantBuffers()
{
    // GPU Resource
    bufferList_.resize(NUM_OF_BUFFER);
    glGenBuffers(static_cast<GLsizei>(bufferList_.size()), bufferList_.data());
    PerEngineBuffer staticConstantBuffer;
    staticConstantBuffer.viewToProjection = MakePerspectiveProjectionMatrix(45.0f, static_cast<float>(xResolution_) / yResolution_, 1.0f, 100.0f);
    
    glBindBuffer(GL_UNIFORM_BUFFER, bufferList_[PER_ENGINE_BUFFER]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PerEngineBuffer), &staticConstantBuffer, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferList_[PER_FRAME_BUFFER]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PerFrameBuffer), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferList_[PER_OBJECT_BUFFER]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PerObjectBuffer), nullptr, GL_DYNAMIC_DRAW);
    for (int i = 0; i < bufferList_.size(); i++) {
        glBindBufferBase(GL_UNIFORM_BUFFER, i, bufferList_[i]);
    }
}
void RenderEngine::SetupPreIntegratedData()
{
    // load dfg
//    auto dib = FreeImage_Load(FIF_EXR, "../../Resources/Environment/dfg.exr");
//    auto w = FreeImage_GetWidth(dib);
//    auto h = FreeImage_GetHeight(dib);
//    auto bits = FreeImage_GetBits(dib);
//    glCreateTextures(GL_TEXTURE_2D, 1, &dfgTextureID_);
//    glTextureStorage2D(dfgTextureID_, 1, GL_RGB16F, w, h);
//    glTextureSubImage2D(dfgTextureID_, 0, 0, 0, w, h, GL_RGB, GL_FLOAT, bits);
//    FreeImage_Unload(dib);
//    glBindTextureUnit(0, dfgTextureID_);
//    glBindSampler(0, defaultSamplerID_);
//    std::cerr << "DFG: " << w << "," << h << std::endl;
}
}
