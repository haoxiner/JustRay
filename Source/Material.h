#pragma once
#include "Platform.h"
#include <string>
#include <vector>
namespace JustRay
{
class Material
{
public:
    enum Type
    {
        CONDUCTOR, DIELECTRIC, CONDUCTOR_AND_DIELECTRIC
    };
    Material(const std::string& name);
    ~Material();
    void Use();
private:
    void LoadTexture(GLenum internalFormat, GLenum format, GLenum type, unsigned int width, unsigned int height, void* data);
    std::vector<GLuint> textureIDList_;
};
}
