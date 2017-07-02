#pragma once
#include "MathUtil.h"
#include "Platform.h"
#include <string>
#include <vector>
namespace JustRay
{
class Material
{
friend class RenderEngine;
public:
    Material(float r, float g, float b, float metallic, float roughness, float texCoordScale = 1.0);
    Material(const std::string& name, float texCoordScale = 1.0f);
    ~Material();
    void Set(bool useTexture, float r, float g, float b, float metallic, float roughness, float texCoordScale);
    void SetTexCoordScale(float scale) { custom[0].y = scale; }
    void Use(int unit);
    float GetUVScale() { return custom[0].y; }
private:
    void LoadTexture(GLenum internalFormat, GLenum format, GLenum type, unsigned int width, unsigned int height, void* data);
    std::vector<GLuint> textureIDList_;
    Float4 custom[2];
};
}
