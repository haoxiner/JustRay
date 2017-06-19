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
Material::Material(const std::string& name)
{
    const std::string directory = "Material/" + name + "/";
    Json::JsonObject config;
    Json::Parser parser(&config, ResourceLoader::LoadFileAsString(directory + "material.json"));
    std::string type = config.GetValue("type").AsString();
    std::cerr << type << std::endl;
    
    short header[4];
    
    std::ifstream baseColorFile(directory + "basecolor.raw", std::ios::binary);
    baseColorFile.read(reinterpret_cast<char*>(header), sizeof(header));
    std::vector<unsigned char> baseColorMap(header[0] * header[1] * header[2]);
    baseColorFile.read(reinterpret_cast<char*>(baseColorMap.data()), baseColorMap.size());
    baseColorFile.close();
    LoadTexture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, header[0], header[1], baseColorMap.data());
    
    std::ifstream roughnessFile(directory + "roughness.raw", std::ios::binary);
    roughnessFile.read(reinterpret_cast<char*>(header), sizeof(header));
    std::vector<unsigned char> roughnessMap(header[0] * header[1] * header[2]);
    roughnessFile.read(reinterpret_cast<char*>(roughnessMap.data()), roughnessMap.size());
    roughnessFile.close();
    LoadTexture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, header[0], header[1], roughnessMap.data());
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
    glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, type, data);
}
}
