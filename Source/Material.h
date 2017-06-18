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
    std::vector<GLuint> textureIDList_;
};
}
