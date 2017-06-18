#include "Material.h"
#include "Json.h"
#include <iostream>
namespace JustRay
{
Material::Material(const std::string& name)
{
    const std::string directory = "../../Resources/Material/" + name;

//    Json::JsonObject config;
//    Json::Parser parser(&config, ResourceManager::ReadFileToString(directory + "/material.json"));
//    std::string type = config.GetValue("type").AsString();
//    std::cerr << type << std::endl;
//
//    const std::string dataList[] = { "basecolor.hdr", "roughness.hdr" };
//    const GLenum dataTypeList[] = { GL_RGB8, GL_R8 };

}

Material::~Material()
{
//    glDeleteTextures(textureIDList_.size(), textureIDList_.data());
}

void Material::Use()
{
    for (int i = 0; i < textureIDList_.size(); i++) {
//        glBindTextureUnit(3 + i, textureIDList_[i]);
    }
}
}
