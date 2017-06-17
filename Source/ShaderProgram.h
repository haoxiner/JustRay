#pragma once
#include "Platform.h"
#include <string>
namespace JustRay
{
class ShaderProgram
{
public:
    static GLuint CompileShader(GLuint vertexShaderID, GLuint fragmentShaderID);
    static GLuint LoadShader(const std::string& source, GLenum type);
};
}
