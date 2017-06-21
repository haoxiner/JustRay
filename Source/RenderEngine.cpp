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
    
    glBindSampler(0, defaultSamplerID_);
    glBindSampler(3, repeatSamplerID_);
    glBindSampler(4, repeatSamplerID_);
    glBindSampler(5, repeatSamplerID_);

    SetupShader();
    SetupConstantBuffers();
    SetupPreIntegratedData();
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
            material->Use();
            perObjectBuffer.material[0] = material->custom[0];
            perObjectBuffer.material[1] = material->custom[1];
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(perObjectBuffer), &perObjectBuffer);
            glDrawElements(GL_TRIANGLES, numOfIndex, modelGroup.indexType_, reinterpret_cast<GLvoid*>(indexOffset));
        }
    }
}

void RenderEngine::SetupShader()
{
    auto vertexShaderID = ShaderProgram::LoadShader(ResourceLoader::LoadFileAsString("Shader/Stationary.vert.glsl"), GL_VERTEX_SHADER);
    auto fragmentShaderID = ShaderProgram::LoadShader(ResourceLoader::LoadFileAsString("Shader/pbr.frag.glsl"), GL_FRAGMENT_SHADER);
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
    for (int i = 0; i < bufferList_.size(); i++) {
        glBindBufferBase(GL_UNIFORM_BUFFER, i, bufferList_[i]);
        unsigned int blockIndex = glGetUniformBlockIndex(pbrShaderForStationaryEntity_, BUFFER_NAMES[i].c_str());
        glUniformBlockBinding(pbrShaderForStationaryEntity_, blockIndex, i);
    }
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
    std::cerr << glGetError() << std::endl;
}
}
