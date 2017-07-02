//
//  AOBuffer.cpp
//  JustRay
//
//  Created by xin on 2017/6/26.
//
//

#include "AOBuffer.h"
#include "MathUtil.h"
#include "ShaderProgram.h"
#include "ResourceLoader.h"
#include <vector>
#include <random>
#include <iostream>
namespace JustRay
{
static const int kernelSize = 64;
struct SSAOUniformBuffer
{
    Float4 settings;
    Float4 kernel[kernelSize];
};
AOBuffer::AOBuffer(int xResolution, int yResolution)
{
    screenWidth_ = xResolution;
    screenHeight_ = yResolution;
    
    occlusionWidth_ = xResolution / 2;
    occlusionHeight_ = yResolution / 2;
    
    std::uniform_real_distribution<float> randomFloats(-1.0f,1.0f);
    std::default_random_engine generator;
    
    std::vector<Float4> kernel(kernelSize);
    for (int i = 0; i < kernelSize; i++) {
        kernel[i].x = randomFloats(generator);
        kernel[i].y = randomFloats(generator);
        kernel[i].z = 1.0f;
        kernel[i].w = 0.0f;
        kernel[i] = Normalize(kernel[i]);
        kernel[i] *= (randomFloats(generator) * 0.5f + 0.5f);
        float scale = static_cast<float>(i) / kernelSize;
        scale = Lerp(0.1f, 1.0f, scale * scale);
        kernel[i] *= scale;
    }
    
    const int noiseWidth = 4;
    std::vector<Float2> noise(noiseWidth * noiseWidth);
    for (int i = 0; i < noise.size(); i++) {
        noise[i].x = randomFloats(generator);
        noise[i].y = randomFloats(generator);
        noise[i] = Normalize(noise[i]);
    }
    glGenTextures(1, &noiseBuffer_);
    glBindTexture(GL_TEXTURE_2D, noiseBuffer_);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG16F, noiseWidth, noiseWidth);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, noiseWidth, noiseWidth, GL_RG, GL_FLOAT, reinterpret_cast<GLvoid*>(noise.data()));
    
    
    // shader and ubos
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
    Float4 settings(occlusionWidth_/noiseWidth, occlusionHeight_/noiseWidth, 1.0f, 1.0f);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Float4), &settings);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Float4), kernel.size() * sizeof(Float4), kernel.data());
    
    // frame buffer
    glGenTextures(1, &occlusionBuffer_);
    glBindTexture(GL_TEXTURE_2D, occlusionBuffer_);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG16F, occlusionWidth_, occlusionHeight_);
    
    GLuint originalFrameBufferID;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*>(&originalFrameBufferID));
    glGenFramebuffers(1, &occlusionFBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, occlusionFBO_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, occlusionBuffer_, 0);
    
    
    // blur X
    glGenTextures(1, &blurXBuffer_);
    glBindTexture(GL_TEXTURE_2D, blurXBuffer_);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, occlusionWidth_, occlusionHeight_);
    glGenFramebuffers(1, &blurXFBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, blurXFBO_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurXBuffer_, 0);
    // shader
    fragmentShaderID = ShaderProgram::LoadShader(ResourceLoader::LoadFileAsString("Shader/SSAOBlurX.frag.glsl"), GL_FRAGMENT_SHADER);
    blurXShader_ = ShaderProgram::CompileShader(vertexShaderID, fragmentShaderID);
    glDeleteShader(fragmentShaderID);
    glUseProgram(blurXShader_);
    glUniform1i(glGetUniformLocation(blurXShader_, "occlusionBuffer"), 0);
    // blur Y
    glGenTextures(1, &blurYBuffer_);
    glBindTexture(GL_TEXTURE_2D, blurYBuffer_);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, occlusionWidth_, occlusionHeight_);
    glGenFramebuffers(1, &blurYFBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, blurYFBO_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurYBuffer_, 0);
    // shader
    fragmentShaderID = ShaderProgram::LoadShader(ResourceLoader::LoadFileAsString("Shader/SSAOBlurY.frag.glsl"), GL_FRAGMENT_SHADER);
    blurYShader_ = ShaderProgram::CompileShader(vertexShaderID, fragmentShaderID);
    glDeleteShader(fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glUseProgram(blurYShader_);
    glUniform1i(glGetUniformLocation(blurYShader_, "occlusionBuffer"), 0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, originalFrameBufferID);
    
}
AOBuffer::~AOBuffer()
{
    glDeleteBuffers(1, &uniformBufferID_);
    
    glDeleteTextures(1, &occlusionBuffer_);
    glDeleteTextures(1, &noiseBuffer_);
    glDeleteTextures(1, &blurXBuffer_);
    glDeleteTextures(1, &blurYBuffer_);
    
    glDeleteProgram(occlusionShader_);
    glDeleteProgram(blurXShader_);
    glDeleteProgram(blurYBuffer_);
    
    glDeleteFramebuffers(1, &occlusionFBO_);
    glDeleteFramebuffers(1, &blurXFBO_);
    glDeleteFramebuffers(1, &blurYFBO_);
}
void AOBuffer::CalculateOcclusion(JustRay::Texture2DSampler &texture2DSampler, GLuint quadVertexArrayID, GLuint normalBufferID, GLuint depthBufferID)
{
    glBindFramebuffer(GL_FRAMEBUFFER, occlusionFBO_);
    glViewport(0, 0, occlusionWidth_, occlusionHeight_);
    glUseProgram(occlusionShader_);
    texture2DSampler.UsePointSampler(0);
    texture2DSampler.UsePointSampler(1);
    texture2DSampler.UsePointRepeatSampler(2);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, normalBufferID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthBufferID);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, noiseBuffer_);
    glBindVertexArray(quadVertexArrayID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glBindFramebuffer(GL_FRAMEBUFFER, blurXFBO_);
    glViewport(0, 0, occlusionWidth_, occlusionHeight_);
    glUseProgram(blurXShader_);
    texture2DSampler.UsePointMirrorRepeatSampler(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, occlusionBuffer_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glBindFramebuffer(GL_FRAMEBUFFER, blurYFBO_);
    glViewport(0, 0, occlusionWidth_, occlusionHeight_);
    glUseProgram(blurYShader_);
    texture2DSampler.UsePointMirrorRepeatSampler(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, blurXBuffer_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void AOBuffer::UseOcclusionBufferAsTexture(int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, blurYBuffer_);
}
};
