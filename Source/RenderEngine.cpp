//
//  RenderEngine.cpp
//  JustRay
//
//  Created by xin on 2017/6/17.
//
//

#include "RenderEngine.h"
#include "ResourceLoader.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>

namespace JustRay
{
struct PerObjectBuffer
{
    Matrix4x4 modelToWorld;
    Float4 material[2];
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
static std::string BUFFER_NAMES[] = {"PerEngineBuffer", "PerFrameBuffer", "PerObjectBuffer"};
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
    glBindSampler(0, repeatSamplerID_);
    glBindSampler(1, repeatSamplerID_);
    glBindSampler(3, repeatSamplerID_);
    glBindSampler(4, repeatSamplerID_);
    glBindSampler(5, repeatSamplerID_);

    SetupShader();
    SetupConstantBuffers();
    SetupPreIntegratedData();
//    SetupEnvironment("uffizi");
}
void RenderEngine::Render(const ModelGroup& modelGroup)
{
    glUseProgram(pbrShaderForStationaryEntity_);
    glBindVertexArray(modelGroup.vertexArrayID_);
    for (const auto& model : modelGroup.models_) {
        int indexOffset = std::get<0>(model);
        int numOfIndex = std::get<1>(model);
//        int materialIndex = std::get<2>(offset);
        auto& material = std::get<2>(model);
        material->Use();
        glDrawElements(GL_TRIANGLES, numOfIndex, modelGroup.indexType_, reinterpret_cast<GLvoid*>(indexOffset));
    }
}

void RenderEngine::SetupShader()
{
    auto vertexShaderID = ShaderProgram::LoadShader(ResourceLoader::LoadFileAsString("Shader/Stationary.vert.glsl"), GL_VERTEX_SHADER);
    auto fragmentShaderID = ShaderProgram::LoadShader(ResourceLoader::LoadFileAsString("Shader/test.frag.glsl"), GL_FRAGMENT_SHADER);
    pbrShaderForStationaryEntity_ = ShaderProgram::CompileShader(vertexShaderID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    
    glUseProgram(pbrShaderForStationaryEntity_);
    glUniform1i(glGetUniformLocation(pbrShaderForStationaryEntity_, "basecolorAndRoughnessMap"), 3);
}
void RenderEngine::SetupConstantBuffers()
{
    // GPU Resource
    bufferList_.resize(NUM_OF_BUFFER);
    glGenBuffers(static_cast<GLsizei>(bufferList_.size()), bufferList_.data());
    PerEngineBuffer staticConstantBuffer;
    staticConstantBuffer.viewToProjection = MakePerspectiveProjectionMatrix(45.0f, static_cast<float>(xResolution_) / yResolution_, 0.001f, 100.0f);
    
    PerFrameBuffer perFrameBuffer;
    perFrameBuffer.cameraPosition = Float4(0.0);
    perFrameBuffer.worldToView = glm::lookAtRH(Float3(0,-10,0), Float3(0,1,0), Float3(0,0,1));
    PerObjectBuffer perObjectBuffer;
    perObjectBuffer.modelToWorld = Matrix4x4(1.0);
    
    glBindBuffer(GL_UNIFORM_BUFFER, bufferList_[PER_ENGINE_BUFFER]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PerEngineBuffer), &staticConstantBuffer, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferList_[PER_FRAME_BUFFER]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PerFrameBuffer), &perFrameBuffer, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferList_[PER_OBJECT_BUFFER]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PerObjectBuffer), &perObjectBuffer, GL_DYNAMIC_DRAW);
    for (int i = 0; i < bufferList_.size(); i++) {
        glBindBufferBase(GL_UNIFORM_BUFFER, i, bufferList_[i]);
        unsigned int blockIndex = glGetUniformBlockIndex(pbrShaderForStationaryEntity_, BUFFER_NAMES[i].c_str());
        glUniformBlockBinding(pbrShaderForStationaryEntity_, blockIndex, i);
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
