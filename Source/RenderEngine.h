//
//  RenderEngine.hpp
//  JustRay
//
//  Created by xin on 2017/6/17.
//
//
#pragma once
#include "ShaderProgram.h"
#include "VertexBuffer.h"
#include "Material.h"
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
    void Render(const VertexBuffer& vertexBuffer, const std::vector<std::tuple<int, int, int>>& offsetList, const std::vector<std::shared_ptr<Material>>& materialList);
private:
    void SetupShader();
    void SetupConstantBuffers();
    void SetupPreIntegratedData();
    int xResolution_;
    int yResolution_;
private:
    GLuint pbrShaderForStationaryEntity_;
private:
    GLuint defaultSamplerID_;
    GLuint repeatSamplerID_;
    std::vector<GLuint> bufferList_;
};
}
