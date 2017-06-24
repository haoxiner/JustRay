//
//  DeferredRenderer.cpp
//  JustRay
//
//  Created by xin on 2017/6/24.
//
//

#include "GBuffer.h"
namespace JustRay
{
GBuffer::GBuffer(int xResolution, int yResolution)
{
    GLuint originalFrameBufferID;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*>(&originalFrameBufferID));
    
    glGenFramebuffers(1, &deferredFrameBufferID_);
    glBindFramebuffer(GL_FRAMEBUFFER, deferredFrameBufferID_);
    
    glGenTextures(1, &gBuffer0_);
    glBindTexture(GL_TEXTURE_2D, gBuffer0_);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, xResolution, yResolution);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBuffer0_, 0);
    
    glGenTextures(1, &gBuffer1_);
    glBindTexture(GL_TEXTURE_2D, gBuffer1_);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, xResolution, yResolution);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gBuffer1_, 0);
    
    glGenTextures(1, &gBuffer2_);
    glBindTexture(GL_TEXTURE_2D, gBuffer2_);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB10_A2, xResolution, yResolution);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gBuffer2_, 0);
    
    glGenTextures(1, &depthBufferID_);
    glBindTexture(GL_TEXTURE_2D, depthBufferID_);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, xResolution, yResolution);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBufferID_, 0);
    
    GLenum frameBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, frameBuffers);
    
    glBindFramebuffer(GL_FRAMEBUFFER, originalFrameBufferID);
}
GBuffer::~GBuffer()
{
    glDeleteTextures(1, &depthBufferID_);
    glDeleteTextures(1, &gBuffer2_);
    glDeleteTextures(1, &gBuffer1_);
    glDeleteTextures(1, &gBuffer0_);
    glDeleteFramebuffers(1, &deferredFrameBufferID_);
}
void GBuffer::UseAsRenderTarget()
{
    glBindFramebuffer(GL_FRAMEBUFFER, deferredFrameBufferID_);
}
void GBuffer::UseAsTextures(int unitOffset)
{
    glActiveTexture(GL_TEXTURE0 + unitOffset);
    glBindTexture(GL_TEXTURE_2D, gBuffer0_);
    
    glActiveTexture(GL_TEXTURE1 + unitOffset);
    glBindTexture(GL_TEXTURE_2D, gBuffer1_);
    
    glActiveTexture(GL_TEXTURE2 + unitOffset);
    glBindTexture(GL_TEXTURE_2D, gBuffer2_);
    
    glActiveTexture(GL_TEXTURE3 + unitOffset);
    glBindTexture(GL_TEXTURE_2D, depthBufferID_);
}
void GBuffer::UseNormalBufferAsTexture(int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, gBuffer2_);
}
void GBuffer::UseDepthBufferAsTexture(int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, depthBufferID_);
}
};
