#include "Material.h"
#include "Json.h"
#include "ResourceLoader.h"
#include "AppleSystemTools.h"
#include "lodepng.h"
#include <iostream>
#include <vector>
#include <fstream>

namespace JustRay
{
Material::Material(float r, float g, float b, float metallic, float roughness, float texCoordScale)
{
    Set(false, r, g, b, metallic, roughness, texCoordScale);
}
Material::Material(const std::string& name, float texCoordScale)
{
    const std::string directory = "Material/" + name + "/";
    
    short header[4];
    
    std::ifstream baseColorFile(directory + "basecolor.rgba8", std::ios::binary);
    baseColorFile.read(reinterpret_cast<char*>(header), sizeof(header));
    std::vector<unsigned char> baseColorMap(header[0] * header[1] * header[2]);
    baseColorFile.read(reinterpret_cast<char*>(baseColorMap.data()), baseColorMap.size());
    baseColorFile.close();
    // basecolor encoded in gamma space, metallic in linear space
    LoadTexture(GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE, header[0], header[1], baseColorMap.data());
    
    std::ifstream roughnessFile(directory + "roughness.rgba8", std::ios::binary);
    roughnessFile.read(reinterpret_cast<char*>(header), sizeof(header));
    std::vector<unsigned char> roughnessMap(header[0] * header[1] * header[2]);
    roughnessFile.read(reinterpret_cast<char*>(roughnessMap.data()), roughnessMap.size());
    roughnessFile.close();
    LoadTexture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, header[0], header[1], roughnessMap.data());
    Set(true, 1, 1, 1, 1, 1, texCoordScale);
}

Material::~Material()
{
    glDeleteTextures(static_cast<GLsizei>(textureIDList_.size()), textureIDList_.data());
}

void Material::Use()
{
    for (int i = 0; i < textureIDList_.size(); i++) {
        glActiveTexture(3 + GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textureIDList_[i]);
    }
}
void Material::LoadTexture(GLenum internalFormat, GLenum format, GLenum type, unsigned int width, unsigned int height, void* data)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    textureIDList_.push_back(textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexStorage2D(GL_TEXTURE_2D, 5, internalFormat, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, type, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}
void Material::Set(bool useTexture, float r, float g, float b, float metallic, float roughness, float texCoordScale)
{
    if (useTexture && textureIDList_.size() > 0) {
        custom[0].x = 1.0f;
    } else {
        custom[0].x = 0.0f;
    }
    custom[0].y = texCoordScale;
    custom[0].z = metallic;
    custom[1].x = r;
    custom[1].y = g;
    custom[1].z = b;
    custom[1].w = roughness;
}
}
