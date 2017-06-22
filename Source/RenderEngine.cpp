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

    glGenSamplers(1, &defaultSamplerID_);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(defaultSamplerID_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glGenSamplers(1, &repeatSamplerID_);
    glSamplerParameteri(repeatSamplerID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(repeatSamplerID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(repeatSamplerID_, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(repeatSamplerID_, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(repeatSamplerID_, GL_TEXTURE_MIN_LOD, 0);
    glSamplerParameteri(repeatSamplerID_, GL_TEXTURE_MAX_LOD, 4);
    float aniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
    glSamplerParameterf(repeatSamplerID_, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
    
    glGenSamplers(1, &pointSamplerID_);
    glSamplerParameteri(pointSamplerID_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(pointSamplerID_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(pointSamplerID_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(pointSamplerID_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glBindSampler(0, defaultSamplerID_);
//    glBindSampler(3, repeatSamplerID_);
//    glBindSampler(4, repeatSamplerID_);
//    glBindSampler(5, repeatSamplerID_);

    SetupShader();
    SetupConstantBuffers();
    SetupPreIntegratedData();
    SetupGBuffer();
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

void RenderEngine::RenderToGBuffer(const ModelGroup& modelGroup)
{
    glBindFramebuffer(GL_FRAMEBUFFER, deferredFrameBufferID_);
    
    glBindSampler(0, repeatSamplerID_);
    glBindSampler(1, repeatSamplerID_);
    glUseProgram(deferredShader_);
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
void RenderEngine::Render(const ModelGroup& modelGroup)
{
    glBindSampler(0, defaultSamplerID_);
    glBindSampler(3, repeatSamplerID_);
    glBindSampler(4, repeatSamplerID_);
    glBindSampler(5, repeatSamplerID_);

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
void RenderEngine::Clear()
{
    glBindFramebuffer(GL_FRAMEBUFFER, deferredFrameBufferID_);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void RenderEngine::SubmitToScreen()
{
    glBindFramebuffer(GL_FRAMEBUFFER, screenFrameBufferID_);
    glBindSampler(0, defaultSamplerID_);

    glBindSampler(3, pointSamplerID_);
    glBindSampler(4, pointSamplerID_);
    glBindSampler(5, pointSamplerID_);
    glBindSampler(6, pointSamplerID_);


    glBindVertexArray(squareVertexArrayID_);

    glUseProgram(pbrShader_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dfgTextureID_);
    diffuseCubemap_->Bind(1);
    specularCubemap_->Bind(2);
    
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gBuffer0_);
    
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, gBuffer1_);
    
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, gBuffer2_);
    
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, depthBufferID_);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void RenderEngine::SetupShader()
{
    auto stationaryVertexShaderID = ShaderProgram::LoadShader(ResourceLoader::LoadFileAsString("Shader/Stationary.vert.glsl"), GL_VERTEX_SHADER);
    auto deferredFragmentShaderID = ShaderProgram::LoadShader(ResourceLoader::LoadFileAsString("Shader/deferred.frag.glsl"), GL_FRAGMENT_SHADER);
    deferredShader_ = ShaderProgram::CompileShader(stationaryVertexShaderID, deferredFragmentShaderID);
    
    glDeleteShader(stationaryVertexShaderID);
    glDeleteShader(deferredFragmentShaderID);
    
    glUseProgram(deferredShader_);
    glUniform1i(glGetUniformLocation(deferredShader_, "baseColorMap"), 0);
    glUniform1i(glGetUniformLocation(deferredShader_, "roughnessMap"), 1);
    
    
    
    auto screenVertexShaderID = ShaderProgram::LoadShader(ResourceLoader::LoadFileAsString("Shader/screen.vert.glsl"), GL_VERTEX_SHADER);
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
    staticConstantBuffer.viewToProjection = MakePerspectiveProjectionMatrix(45.0f, static_cast<float>(xResolution_) / yResolution_, 1.0f, 100.0f);
    
    PerFrameBuffer perFrameBuffer;
    perFrameBuffer.cameraPosition = Float4(0,-3,1, 1.0);
    perFrameBuffer.worldToView = glm::lookAtRH(Float3(0,-3,1), Float3(0,0,1), Float3(0,0,1));
    PerObjectBuffer perObjectBuffer;
    perObjectBuffer.modelToWorld = Matrix4x4(1.0);
    perObjectBuffer.material[0] = Float4(2.0,1.0,1.0,1.0);
    perObjectBuffer.material[1] = Float4(1.0);
    
    glBindBuffer(GL_UNIFORM_BUFFER, bufferList_[PER_ENGINE_BUFFER]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PerEngineBuffer), &staticConstantBuffer, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferList_[PER_FRAME_BUFFER]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PerFrameBuffer), &perFrameBuffer, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferList_[PER_OBJECT_BUFFER]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PerObjectBuffer), &perObjectBuffer, GL_DYNAMIC_DRAW);
    
    glUseProgram(deferredShader_);
    for (int i = 0; i < bufferList_.size(); i++) {
        glBindBufferBase(GL_UNIFORM_BUFFER, i, bufferList_[i]);
        unsigned int blockIndex = glGetUniformBlockIndex(deferredShader_, BUFFER_NAMES[i].c_str());
        glUniformBlockBinding(deferredShader_, blockIndex, i);
    }
    glUseProgram(pbrShader_);
    for (int i = 0; i < bufferList_.size() - 1; i++) {
        glBindBufferBase(GL_UNIFORM_BUFFER, i, bufferList_[i]);
        unsigned int blockIndex = glGetUniformBlockIndex(pbrShader_, BUFFER_NAMES[i].c_str());
        glUniformBlockBinding(pbrShader_, blockIndex, i);
    }
    
    glUseProgram(pbrShaderForStationaryEntity_);
    for (int i = 0; i < bufferList_.size(); i++) {
        glBindBufferBase(GL_UNIFORM_BUFFER, i, bufferList_[i]);
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
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &screenFrameBufferID_);
    
    glGenFramebuffers(1, &deferredFrameBufferID_);
    glBindFramebuffer(GL_FRAMEBUFFER, deferredFrameBufferID_);
    
    glGenTextures(1, &gBuffer0_);
    glBindTexture(GL_TEXTURE_2D, gBuffer0_);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, xResolution_, yResolution_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBuffer0_, 0);
    
    glGenTextures(1, &gBuffer1_);
    glBindTexture(GL_TEXTURE_2D, gBuffer1_);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, xResolution_, yResolution_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gBuffer1_, 0);
    
    glGenTextures(1, &gBuffer2_);
    glBindTexture(GL_TEXTURE_2D, gBuffer2_);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG8, xResolution_, yResolution_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gBuffer2_, 0);
    
    glGenTextures(1, &depthBufferID_);
    glBindTexture(GL_TEXTURE_2D, depthBufferID_);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, xResolution_, yResolution_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBufferID_, 0);
    
    GLenum frameBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, frameBuffers);

    glBindFramebuffer(GL_FRAMEBUFFER, screenFrameBufferID_);

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
