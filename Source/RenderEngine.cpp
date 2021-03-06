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
    aoBuffer_.reset(new AOBuffer(xResolution, yResolution));
    skyBox_.reset(new SkyBox);
}
void RenderEngine::SetEnvironment(const std::string &name)
{
    diffuseCubemap_.reset(new Cubemap("Environment/" + name + "/diffuse.ibl"));
    specularCubemap_.reset(new Cubemap("Environment/" + name + "/specular.ibl"));
}
void RenderEngine::SetCamera(const Float3 &position, const Float3 &focus, const Float3 &up)
{
    perFrameBuffer_.cameraPosition = Float4(position, 1.0);
    perFrameBuffer_.worldToView = glm::lookAtRH(position, focus, up);
}
void RenderEngine::SetEffect(float ao, float exposure)
{
    perFrameBuffer_.perFrameData = Float4(ao,exposure,1.0,1.0);
}
void RenderEngine::Prepare()
{
    glEnable(GL_DEPTH_TEST);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferList_[PER_FRAME_BUFFER]);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(perFrameBuffer_), &perFrameBuffer_);

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
    glBindBuffer(GL_UNIFORM_BUFFER, bufferList_[PER_OBJECT_BUFFER]);
    for (auto&& entity : modelGroup.entities) {
        perObjectBuffer.modelToWorld = QuaternionToMatrix(Normalize(entity->rotation_));
        for (const auto& model : modelGroup.models_) {
            int indexOffset = std::get<0>(model);
            int numOfIndex = std::get<1>(model);
            auto& material = std::get<2>(model);
            material->Use(0);
            perObjectBuffer.material[0] = material->custom[0];
            perObjectBuffer.material[1] = material->custom[1];
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(perObjectBuffer), &perObjectBuffer);
            glDrawElements(GL_TRIANGLES, numOfIndex, modelGroup.indexType_, reinterpret_cast<GLvoid*>(indexOffset * sizeof(unsigned int)));
        }
    }
}
void RenderEngine::Submit(const JustRay::ModelGroup& modelGroup, JustRay::Material& material)
{
    glBindVertexArray(modelGroup.vertexArrayID_);
    material.Use(0);
    PerObjectBuffer perObjectBuffer;
    perObjectBuffer.material[0] = material.custom[0];
    perObjectBuffer.material[1] = material.custom[1];
    glBindBuffer(GL_UNIFORM_BUFFER, bufferList_[PER_OBJECT_BUFFER]);
    for (auto&& entity : modelGroup.entities) {
        perObjectBuffer.modelToWorld = QuaternionToMatrix(Normalize(entity->rotation_));
        for (const auto& model : modelGroup.models_) {
            int indexOffset = std::get<0>(model);
            int numOfIndex = std::get<1>(model);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(perObjectBuffer), &perObjectBuffer);
            glDrawElements(GL_TRIANGLES, numOfIndex, modelGroup.indexType_, reinterpret_cast<GLvoid*>(indexOffset * sizeof(unsigned int)));
        }
    }
}
void RenderEngine::SubmitToScreen()
{
    static bool odd = true;
    if (odd) {
        aoBuffer_->CalculateOcclusion(*texture2DSampler_, squareVertexArrayID_, gBuffer_->GetNormalBufferID(), gBuffer_->GetDepthBufferID());
    }
    odd = !odd;
    
    glBindFramebuffer(GL_FRAMEBUFFER, screenFrameBufferID_);
    glViewport(0, 0, xResolution_, yResolution_);
//    glClearColor(0.0, 0.0, 0.0, 1.0);
//    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(skyShader_);
    specularCubemap_->Bind(2);
    skyBox_->Render();

    glUseProgram(pbrShader_);
    texture2DSampler_->UseDefaultSampler(0);
    texture2DSampler_->UsePointSampler(3);
    texture2DSampler_->UsePointSampler(4);
    texture2DSampler_->UsePointSampler(5);
    texture2DSampler_->UsePointSampler(6);
    texture2DSampler_->UseLinearMirrorRepeatSampler(7);
    glBindVertexArray(squareVertexArrayID_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dfgTextureID_);
    diffuseCubemap_->Bind(1);
    specularCubemap_->Bind(2);
    gBuffer_->UseAsTextures(3);
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
    
    auto skyVertexShaderID = ShaderProgram::LoadShader(ResourceLoader::LoadFileAsString("Shader/SkyBox.vert.glsl"), GL_VERTEX_SHADER);
    auto skyFragmentShaderID = ShaderProgram::LoadShader(ResourceLoader::LoadFileAsString("Shader/SkyBox.frag.glsl"), GL_FRAGMENT_SHADER);
    skyShader_ = ShaderProgram::CompileShader(skyVertexShaderID, skyFragmentShaderID);
    glDeleteShader(skyVertexShaderID);
    glDeleteShader(skyFragmentShaderID);
    glUseProgram(skyShader_);
    glUniform1i(glGetUniformLocation(skyShader_, "specularEnvmap"), 2);
    
    glUseProgram(0);
}
void RenderEngine::SetupConstantBuffers()
{
    // GPU Resource
    bufferList_.resize(NUM_OF_BUFFER);
    glGenBuffers(static_cast<GLsizei>(bufferList_.size()), bufferList_.data());
    perEngineBuffer_.viewToProjection = MakePerspectiveProjectionMatrix(DegreesToRadians(90), static_cast<float>(xResolution_) / yResolution_, 0.1f, 10.0f);
    
    perFrameBuffer_.perFrameData = Float4(1.0, 1.0, 0.0, 0.0);
    perFrameBuffer_.cameraPosition = Float4(0,3,0, 1.0);
    perFrameBuffer_.worldToView = glm::lookAtRH(Float3(0,-3,1), Float3(0,0,1), Float3(0,0,1));
    
    perObjectBuffer_.modelToWorld = Matrix4x4(1.0);
    perObjectBuffer_.material[0] = Float4(1.0,1.0,1.0,1.0);
    perObjectBuffer_.material[1] = Float4(1.0);
    
    glBindBuffer(GL_UNIFORM_BUFFER, bufferList_[PER_ENGINE_BUFFER]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(perEngineBuffer_), &perEngineBuffer_, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferList_[PER_FRAME_BUFFER]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PerFrameBuffer), &perFrameBuffer_, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferList_[PER_OBJECT_BUFFER]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PerObjectBuffer), &perObjectBuffer_, GL_DYNAMIC_DRAW);
    
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
    glUseProgram(skyShader_);
    for (int i = 0; i < bufferList_.size() - 1; i++) {
        unsigned int blockIndex = glGetUniformBlockIndex(skyShader_, BUFFER_NAMES[i].c_str());
        glUniformBlockBinding(skyShader_, blockIndex, i);
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
