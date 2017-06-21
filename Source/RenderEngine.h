//
//  RenderEngine.hpp
//  JustRay
//
//  Created by xin on 2017/6/17.
//
//
#pragma once
#include "ShaderProgram.h"
#include "ModelGroup.h"
#include "Material.h"
#include "Cubemap.h"
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
    void Render(const ModelGroup& modelGroup);
private:
    void SetupShader();
    void SetupConstantBuffers();
    void SetupPreIntegratedData();
    int xResolution_;
    int yResolution_;
private:
    std::unique_ptr<Cubemap> specularCubemap_;
    std::unique_ptr<Cubemap> diffuseCubemap_;
private:
    GLuint pbrShaderForStationaryEntity_;
private:
    GLuint defaultSamplerID_;
    GLuint repeatSamplerID_;
    GLuint dfgTextureID_;
    std::vector<GLuint> bufferList_;
};
}
