//
//  SSAO.cpp
//  JustRay
//
//  Created by xin on 2017/6/24.
//
//

#include "SSAO.h"
#include "MathUtil.h"
#include "ShaderProgram.h"
#include "ResourceLoader.h"
#include <random>
#include <vector>
#include <iostream>
namespace JustRay
{
static const int kernelSize = 64;
struct SSAOUniformBuffer
{
    Float4 settings;
    Float4 kernel[kernelSize];
};
SSAO::SSAO(int xResolution, int yResolution)
{
    width_ = xResolution / 2;
    height_ = yResolution / 2;
    
    std::uniform_real_distribution<float> randomFloats(-1.0f,1.0f);
    std::default_random_engine generator;

    std::vector<Float4> ssaoKernel(kernelSize);
    for (int i = 0; i < kernelSize; i++) {
        Float4 sample(randomFloats(generator), randomFloats(generator), (randomFloats(generator) + 1.0f) * 0.5f, 0.0f);
        ssaoKernel[i] = (Normalize(sample) * ((randomFloats(generator) + 1.0f) * 0.5f));

        float scale = static_cast<float>(i) / static_cast<float>(kernelSize);
        scale = Lerp(0.1f, 1.0f, scale * scale);
        ssaoKernel[i] *= scale;
    }

    const int noiseTextureWidth = 4;
    std::vector<Float2> ssaoNoise;
    for (int i = 0; i < noiseTextureWidth * noiseTextureWidth; i++) {
        ssaoNoise.emplace_back(randomFloats(generator), randomFloats(generator));
    }

    glGenTextures(1, &noiseTextureID_);
    glBindTexture(GL_TEXTURE_2D, noiseTextureID_);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG16F, noiseTextureWidth, noiseTextureWidth);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, noiseTextureWidth, noiseTextureWidth, GL_RG, GL_FLOAT, reinterpret_cast<GLvoid*>(ssaoNoise.data()));
    
    glGenTextures(1, &occlusionBufferID_);
    glBindTexture(GL_TEXTURE_2D, occlusionBufferID_);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, width_, height_);
    
    GLuint originalFrameBufferID;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*>(&originalFrameBufferID));
    glGenFramebuffers(1, &frameBufferID_);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID_);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, occlusionBufferID_, 0);
    
    GLenum frameBuffers[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, frameBuffers);
    
    glBindFramebuffer(GL_FRAMEBUFFER, originalFrameBufferID);
    
    auto vertexShaderID = ShaderProgram::LoadShader(ResourceLoader::LoadFileAsString("Shader/Screen.vert.glsl"), GL_VERTEX_SHADER);
    auto fragmentShaderID = ShaderProgram::LoadShader(ResourceLoader::LoadFileAsString("Shader/SSAO.frag.glsl"), GL_FRAGMENT_SHADER);
    occlusionShader_ = ShaderProgram::CompileShader(vertexShaderID, fragmentShaderID);
    glDeleteShader(fragmentShaderID);
    
    glUseProgram(occlusionShader_);
    glUniform1i(glGetUniformLocation(occlusionShader_, "normalMap"), 0);
    glUniform1i(glGetUniformLocation(occlusionShader_, "depthBuffer"), 1);
    glUniform1i(glGetUniformLocation(occlusionShader_, "noiseMap"), 2);
    
    glUniformBlockBinding(occlusionShader_, glGetUniformBlockIndex(occlusionShader_, "PerEngineBuffer"), 0);
    
    glUniformBlockBinding(occlusionShader_, glGetUniformBlockIndex(occlusionShader_, "PerFrameBuffer"), 1);
    
    glGenBuffers(1, &uniformBufferID_);
    glBindBuffer(GL_UNIFORM_BUFFER, uniformBufferID_);
    glBindBufferBase(GL_UNIFORM_BUFFER, 3, uniformBufferID_);
    glUniformBlockBinding(occlusionShader_, glGetUniformBlockIndex(occlusionShader_, "SSAOBuffer"), 3);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SSAOUniformBuffer), nullptr, GL_DYNAMIC_DRAW);
    
    Float4 settings(width_/4, height_/4, 1.0f, 1.0f);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Float4), &settings);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Float4), ssaoKernel.size() * sizeof(Float4), ssaoKernel.data());
    
    
    glGenTextures(1, &bluredOcclusionBufferID_);
    glBindTexture(GL_TEXTURE_2D, bluredOcclusionBufferID_);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, width_, height_);
    glGenFramebuffers(1, &bluredFrameBufferID_);
    glBindFramebuffer(GL_FRAMEBUFFER, bluredFrameBufferID_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bluredOcclusionBufferID_, 0);
    glDrawBuffers(1, frameBuffers);
    glBindFramebuffer(GL_FRAMEBUFFER, originalFrameBufferID);
    
    fragmentShaderID = ShaderProgram::LoadShader(ResourceLoader::LoadFileAsString("Shader/SSAOBlur.frag.glsl"), GL_FRAGMENT_SHADER);
    blurShader_ = ShaderProgram::CompileShader(vertexShaderID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    glUseProgram(blurShader_);
    glUniform1i(glGetUniformLocation(blurShader_, "occlusionBuffer"), 0);
    
    glGenTextures(1, &resultOcclusionBufferID_);
    glBindTexture(GL_TEXTURE_2D, resultOcclusionBufferID_);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, xResolution, yResolution);
    glGenFramebuffers(1, &resultFrameBufferID_);
    glBindFramebuffer(GL_FRAMEBUFFER, resultFrameBufferID_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resultOcclusionBufferID_, 0);
    glDrawBuffers(1, frameBuffers);
    glBindFramebuffer(GL_FRAMEBUFFER, originalFrameBufferID);
}
SSAO::~SSAO()
{
    glDeleteBuffers(1, &uniformBufferID_);
    glDeleteTextures(1, &occlusionBufferID_);
    glDeleteTextures(1, &noiseTextureID_);
    glDeleteProgram(occlusionShader_);
}
void SSAO::CalculateOcclusion(Texture2DSampler &texture2DSampler, GLuint quadVertexArrayID, GLuint normalBufferID, GLuint depthBufferID)
{
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID_);
    glViewport(0, 0, width_, height_);
    glUseProgram(occlusionShader_);
    texture2DSampler.UsePointSampler(0);
    texture2DSampler.UseDefaultSampler(1);
    texture2DSampler.UsePointRepeatSampler(2);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, normalBufferID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthBufferID);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, noiseTextureID_);
    glBindVertexArray(quadVertexArrayID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glViewport(0, 0, width_ * 2, height_ * 2);
    glBindFramebuffer(GL_FRAMEBUFFER, resultFrameBufferID_);
    glUseProgram(blurShader_);
    texture2DSampler.UsePointSampler(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, occlusionBufferID_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
//    glBindFramebuffer(GL_FRAMEBUFFER, bluredFrameBufferID_);
//    glUseProgram(blurShader_);
//    glUniform2f(glGetUniformLocation(blurShader_, "direction"), 1.0f, 0.0f);
//    texture2DSampler.UsePointSampler(0);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, occlusionBufferID_);
//    glDrawArrays(GL_TRIANGLES, 0, 6);
    
//    glUniform2f(glGetUniformLocation(blurShader_, "direction"), 0.0f, 1.0f);
//    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID_);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, bluredOcclusionBufferID_);
//    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void SSAO::UseOcclusionBufferAsTexture(int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, occlusionBufferID_);
}
void SSAO::UseBluredOcclusionBufferAsTexture(int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, resultOcclusionBufferID_);
}
};
