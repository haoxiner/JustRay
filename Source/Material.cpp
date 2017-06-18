#include "Material.h"
#include "Json.h"
#include "ResourceLoader.h"
#include "AppleSystemTools.h"
#include "lodepng.h"
#include <iostream>
#include <vector>
namespace JustRay
{
Material::Material(const std::string& name)
{
    const std::string directory = "Material/" + name + "/";

    Json::JsonObject config;
    Json::Parser parser(&config, ResourceLoader::LoadFileAsString(directory + "material.json"));
    std::string type = config.GetValue("type").AsString();
    std::cerr << type << std::endl;
    
    unsigned int width = 0;
    unsigned int height = 0;
    std::vector<unsigned char> basecolorMap;
    lodepng::decode(basecolorMap, width, height, GetFilePath(directory + "basecolor.png").c_str());
    std::vector<unsigned char> roughnessMap;
    lodepng::decode(roughnessMap, width, height, GetFilePath(directory + "roughness.png").c_str());
    for (int i = 0; i < basecolorMap.size(); i+=4) {
        basecolorMap[i + 3] = roughnessMap[i];
    }
    LoadTexture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, width, height, basecolorMap.data());
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
