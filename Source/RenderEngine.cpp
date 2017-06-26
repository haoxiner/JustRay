//
//  RenderEngine.cpp
//  JustRay
//
//  Created by xin on 2017/6/17.
//
//

#include "RenderEngine.h"
#include "ResourceLoader.h"
#include "AppleSystemTools.h"
#include "half.h"
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
    texture2DSampler_.reset(new Texture2DSampler);
    SetupShader();
    SetupConstantBuffers();
    SetupPreIntegratedData();
    SetupGBuffer();
//    ssao_.reset(new SSAO(xResolution, yResolution));
    aoBuffer_.reset(new AOBuffer(xResolution, yResolution));
}
void RenderEngine::SetEnvironment(const std::string &name)
{
    diffuseCubemap_.reset(new Cubemap("Environment/" + name + "/diffuse.ibl"));
    specularCubemap_.reset(new Cubemap("Environment/" + name + "/specular.ibl"));
}
void RenderEngine::SetCamera(const Float3 &position, const Float3 &focus, const Float3 &up)
{
    PerFrameBuffer perFrameBuffer;
    perFrameBuffer.cameraPosition = Float4(position, 1.0);
    perFrameBuffer.worldToView = glm::lookAtRH(position, focus, up);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferList_[PER_FRAME_BUFFER]);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(perFrameBuffer), &perFrameBuffer);
}
void RenderEngine::Render(const ModelGroup& modelGroup)
{
    texture2DSampler_->UseDefaultSampler(0);
    texture2DSampler_->UseRepeatSampler(3);
    texture2DSampler_->UseRepeatSampler(4);
    texture2DSampler_->UseRepeatSampler(5);
    
    glUseProgram(pbrShaderForStationaryEntity_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dfgTextureID_);
    diffuseCubemap_->Bind(1);
    specularCubemap_->Bind(2);
    glBindVertexArray(modelGroup.vertexArrayID_);
    
    for (auto&& entity : modelGroup.entities) {
        PerObjectBuffer perObjectBuffer;
        perObjectBuffer.modelToWorld = QuaternionToMatrix(Normalize(entity->rotation_));
        glBindBuffer(GL_UNIFORM_BUFFER, bufferList_[PER_OBJECT_BUFFER]);
        for (const auto& model : modelGroup.models_) {
            int indexOffset = std::get<0>(model);
            int numOfIndex = std::get<1>(model);
            auto& material = std::get<2>(model);
            material->Use(3);
            perObjectBuffer.material[0] = material->custom[0];
            perObjectBuffer.material[1] = material->custom[1];
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(perObjectBuffer), &perObjectBuffer);
            glDrawElements(GL_TRIANGLES, numOfIndex, modelGroup.indexType_, reinterpret_cast<GLvoid*>(indexOffset));
        }
    }
}
void RenderEngine::Prepare()
{
    gBuffer_->UseAsRenderTarget();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    texture2DSampler_->UseRepeatSampler(0);
    texture2DSampler_->UseRepeatSampler(1);
    glUseProgram(gBufferShader_);
}
void RenderEngine::Submit(const JustRay::ModelGroup& modelGroup)
{
    glBindVertexArray(modelGroup.vertexArrayID_);
    PerObjectBuffer perObjectBuffer;
    for (auto&& entity : modelGroup.entities) {
        perObjectBuffer.modelToWorld = QuaternionToMatrix(Normalize(entity->rotation_));
        glBindBuffer(GL_UNIFORM_BUFFER, bufferList_[PER_OBJECT_BUFFER]);
        for (const auto& model : modelGroup.models_) {
            int indexOffset = std::get<0>(model);
            int numOfIndex = std::get<1>(model);
            auto& material = std::get<2>(model);
            material->Use(0);
            perObjectBuffer.material[0] = material->custom[0];
            perObjectBuffer.material[1] = material->custom[1];
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(perObjectBuffer), &perObjectBuffer);
            glDrawElements(GL_TRIANGLES, numOfIndex, modelGroup.indexType_, reinterpret_cast<GLvoid*>(indexOffset));
        }
    }
}
void RenderEngine::SubmitToScreen()
{
//    static bool odd = true;
//    if (odd) {
//        ssao_->CalculateOcclusion(*texture2DSampler_, squareVertexArrayID_, gBuffer_->GetNormalBufferID(), gBuffer_->GetDepthBufferID());
//    }
//    odd = true;
    aoBuffer_->CalculateOcclusion(*texture2DSampler_, squareVertexArrayID_, gBuffer_->GetNormalBufferID(), gBuffer_->GetDepthBufferID());
    
    glBindFramebuffer(GL_FRAMEBUFFER, screenFrameBufferID_);
    glViewport(0, 0, xResolution_, yResolution_);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    texture2DSampler_->UseDefaultSampler(0);
    texture2DSampler_->UsePointSampler(3);
    texture2DSampler_->UsePointSampler(4);
    texture2DSampler_->UsePointSampler(5);
    texture2DSampler_->UsePointSampler(6);
    texture2DSampler_->UsePointSampler(7);
    glBindVertexArray(squareVertexArrayID_);
    glUseProgram(pbrShader_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dfgTextureID_);
    diffuseCubemap_->Bind(1);
    specularCubemap_->Bind(2);
    gBuffer_->UseAsTextures(3);
//    ssao_->UseBluredOcclusionBufferAsTexture(7);
    aoBuffer_->UseOcclusionBufferAsTexture(7);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void RenderEngine::SetupShader()
{
    auto stationaryVertexShaderID = ShaderProgram::LoadShader(ResourceLoader::LoadFileAsString("Shader/Stationary.vert.glsl"), GL_VERTEX_SHADER);
    auto deferredFragmentShaderID = ShaderProgram::LoadShader(ResourceLoader::LoadFileAsString("Shader/GBuffer.frag.glsl"), GL_FRAGMENT_SHADER);
    gBufferShader_ = ShaderProgram::CompileShader(stationaryVertexShaderID, deferredFragmentShaderID);
    
    glDeleteShader(stationaryVertexShaderID);
    glDeleteShader(deferredFragmentShaderID);
    
    glUseProgram(gBufferShader_);
    glUniform1i(glGetUniformLocation(gBufferShader_, "baseColorMap"), 0);
    glUniform1i(glGetUniformLocation(gBufferShader_, "roughnessMap"), 1);
    
    auto screenVertexShaderID = ShaderProgram::LoadShader(ResourceLoader::LoadFileAsString("Shader/Screen.vert.glsl"), GL_VERTEX_SHADER);
    auto pbrFragmentShaderID = ShaderProgram::LoadShader(ResourceLoader::LoadFileAsString("Shader/PBR.frag.glsl"), GL_FRAGMENT_SHADER);
    pbrShader_ = ShaderProgram::CompileShader(screenVertexShaderID, pbrFragmentShaderID);
    
    glDeleteShader(screenVertexShaderID);
    glDeleteShader(pbrFragmentShaderID);
    
    glUseProgram(pbrShader_);
    glUniform1i(glGetUniformLocation(pbrShader_, "dfgMap"), 0);
    glUniform1i(glGetUniformLocation(pbrShader_, "diffuseEnvmap"), 1);
    glUniform1i(glGetUniformLocation(pbrShader_, "specularEnvmap"), 2);
    glUniform1i(glGetUniformLocation(pbrShader_, "gBuffer0"), 3);
    glUniform1i(glGetUniformLocation(pbrShader_, "gBuffer1"), 4);
    glUniform1i(glGetUniformLocation(pbrShader_, "gBuffer2"), 5);
    glUniform1i(glGetUniformLocation(pbrShader_, "depthBuffer"), 6);
    glUniform1i(glGetUniformLocation(pbrShader_, "occlusionBuffer"), 7);
    
    glUseProgram(0);
    
    auto vertexShaderID = ShaderProgram::LoadShader(ResourceLoader::LoadFileAsString("Shader/Stationary.vert.glsl"), GL_VERTEX_SHADER);
    auto fragmentShaderID = ShaderProgram::LoadShader(ResourceLoader::LoadFileAsString("Shader/PBRForward.frag.glsl"), GL_FRAGMENT_SHADER);
    pbrShaderForStationaryEntity_ = ShaderProgram::CompileShader(vertexShaderID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    
    glUseProgram(pbrShaderForStationaryEntity_);
    glUniform1i(glGetUniformLocation(pbrShaderForStationaryEntity_, "dfgMap"), 0);
    glUniform1i(glGetUniformLocation(pbrShaderForStationaryEntity_, "diffuseEnvmap"), 1);
    glUniform1i(glGetUniformLocation(pbrShaderForStationaryEntity_, "specularEnvmap"), 2);
    glUniform1i(glGetUniformLocation(pbrShaderForStationaryEntity_, "baseColorMap"), 3);
    glUniform1i(glGetUniformLocation(pbrShaderForStationaryEntity_, "roughnessMap"), 4);
}
void RenderEngine::SetupConstantBuffers()
{
    // GPU Resource
    bufferList_.resize(NUM_OF_BUFFER);
    glGenBuffers(static_cast<GLsizei>(bufferList_.size()), bufferList_.data());
    PerEngineBuffer staticConstantBuffer;
    staticConstantBuffer.viewToProjection = MakePerspectiveProjectionMatrix(45.0f, static_cast<float>(xResolution_) / yResolution_, 1.0f, 50.0f);
    
    PerFrameBuffer perFrameBuffer;
    perFrameBuffer.cameraPosition = Float4(0,-3,1, 1.0);
    perFrameBuffer.worldToView = glm::lookAtRH(Float3(0,-3,1), Float3(0,0,1), Float3(0,0,1));
    PerObjectBuffer perObjectBuffer;
    perObjectBuffer.modelToWorld = Matrix4x4(1.0);
    perObjectBuffer.material[0] = Float4(1.0,1.0,1.0,1.0);
    perObjectBuffer.material[1] = Float4(1.0);
    
    glBindBuffer(GL_UNIFORM_BUFFER, bufferList_[PER_ENGINE_BUFFER]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PerEngineBuffer), &staticConstantBuffer, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferList_[PER_FRAME_BUFFER]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PerFrameBuffer), &perFrameBuffer, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferList_[PER_OBJECT_BUFFER]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PerObjectBuffer), &perObjectBuffer, GL_DYNAMIC_DRAW);
    
    for (int i = 0; i < bufferList_.size(); i++) {
        glBindBufferBase(GL_UNIFORM_BUFFER, i, bufferList_[i]);
    }
    
    glUseProgram(gBufferShader_);
    for (int i = 0; i < bufferList_.size(); i++) {
        unsigned int blockIndex = glGetUniformBlockIndex(gBufferShader_, BUFFER_NAMES[i].c_str());
        glUniformBlockBinding(gBufferShader_, blockIndex, i);
    }
    glUseProgram(pbrShader_);
    for (int i = 0; i < bufferList_.size() - 1; i++) {
        unsigned int blockIndex = glGetUniformBlockIndex(pbrShader_, BUFFER_NAMES[i].c_str());
        glUniformBlockBinding(pbrShader_, blockIndex, i);
    }
    
    glUseProgram(pbrShaderForStationaryEntity_);
    for (int i = 0; i < bufferList_.size(); i++) {
        unsigned int blockIndex = glGetUniformBlockIndex(pbrShaderForStationaryEntity_, BUFFER_NAMES[i].c_str());
        glUniformBlockBinding(pbrShaderForStationaryEntity_, blockIndex, i);
    }
    glUseProgram(0);
}
void RenderEngine::SetupPreIntegratedData()
{
    // load dfg
    short header[4];
    std::ifstream file(GetFilePath("Environment/dfg.ibl.float"), std::ios::binary);
    file.read(reinterpret_cast<char*>(header), sizeof(header));
    int width = header[0];
    int height = header[1];
    std::vector<float> floatData(width * height * header[2]);
    file.read(reinterpret_cast<char*>(floatData.data()), floatData.size()*sizeof(float));
    file.close();
    glGenTextures(1, &dfgTextureID_);
    glBindTexture(GL_TEXTURE_2D, dfgTextureID_);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, floatData.data());
}
void RenderEngine::SetupGBuffer()
{
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*>(&screenFrameBufferID_));
    gBuffer_.reset(new GBuffer(xResolution_, yResolution_));

    float square[] = {
        -1.0f, +1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        +1.0f, +1.0f, 1.0f, 1.0f,
        
        +1.0f, +1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        +1.0f, -1.0f, 1.0f, 0.0f
    };
    glGenVertexArrays(1, &squareVertexArrayID_);
    glBindVertexArray(squareVertexArrayID_);
    glGenBuffers(1, &squareVertexBufferID_);
    glBindBuffer(GL_ARRAY_BUFFER, squareVertexBufferID_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
    glBindVertexArray(0);
}
}
