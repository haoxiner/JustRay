//
//  Mesh.cpp
//  JustRay
//
//  Created by xin on 2017/6/17.
//
//

#include "ModelGroup.h"
#include "Json.h"
#include "ResourceLoader.h"
#include "AppleSystemTools.h"
#include "Vertex.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <utility>
namespace JustRay
{
ModelGroup::ModelGroup(const std::string& name, std::map<const std::string, std::shared_ptr<Material>>& materialMap)
{
    Json::JsonObject json;
    Json::Parser parser(&json, ResourceLoader::LoadFileAsString("Model/" + name + ".json"));
    if (!parser.HasError()) {
        auto numOfVertex = json.GetValue("num_of_vertex").AsInt();
        auto numOfIndex = json.GetValue("num_of_index").AsInt();
    
        auto modelArrayObject = json.GetValue("model");
        auto modelArray = modelArrayObject.AsJsonArray();
        int modelArraySize = modelArrayObject.GetType();
        std::ifstream file(GetFilePath("Model/" + name + ".mg"), std::ios::binary);
        
        std::vector<Vertex> vertices(numOfVertex);
        std::vector<unsigned int> indices(numOfIndex);
        int sizeOfVertexData = numOfVertex * sizeof(Vertex);
        file.read(reinterpret_cast<char*>(vertices.data()), sizeOfVertexData);
        std::cerr << sizeOfVertexData << std::endl;
        int sizeOfIndexData = numOfIndex * sizeof(unsigned int);
        file.read(reinterpret_cast<char*>(indices.data()), sizeOfIndexData);
        Load(vertices.data(), sizeOfVertexData,
            {{3, GL_FLOAT, false, 3 * sizeof(float)},
            {4, GL_INT_2_10_10_10_REV, true, sizeof(Int_2_10_10_10)},
            {4, GL_INT_2_10_10_10_REV, true, sizeof(Int_2_10_10_10)},
            {4, GL_INT_2_10_10_10_REV, true, sizeof(Int_2_10_10_10)},
            {2, GL_HALF_FLOAT, false, sizeof(half) * 2}},
            indices.data(), sizeOfIndexData);
        for (int i = 0; i < modelArraySize; i++) {
            auto modelInfo = modelArray[i].AsJsonObject();
            int indexOffset = modelInfo->GetValue("index_offset").AsInt();
            int indexCount = modelInfo->GetValue("index_count").AsInt();
            auto materialName = modelInfo->GetValue("material").AsString();
            float uvScale = modelInfo->GetValue("uv_scale").AsFloat();
            auto iter = materialMap.find(materialName);
            if (iter != materialMap.end()) {
                models_.emplace_back(indexOffset, indexCount, iter->second);
            } else {
                materialMap[materialName] = std::make_shared<Material>(materialName, uvScale);
                models_.emplace_back(indexOffset, indexCount, materialMap[materialName]);
            }
            
        }
    } else {
        std::cerr << parser.GetErrorMessage() << std::endl;
    }
    
}
ModelGroup::~ModelGroup()
{
    glDeleteBuffers(1, &indexBufferID_);
    glDeleteBuffers(1, &vertexBufferID_);
    glDeleteVertexArrays(1, &vertexArrayID_);
}
void ModelGroup::Load(const void* vertexData, const int sizeOfVertexData, const std::vector<VertexAttributeDescription>& attributeDescriptions, const void* indexData, const int sizeOfIndexData)
{
    glGenVertexArrays(1, &vertexArrayID_);
    glBindVertexArray(vertexArrayID_);
    
    glGenBuffers(1, &vertexBufferID_);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID_);
    
    glGenBuffers(1, &indexBufferID_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID_);
    
    int stride = 0;
    for (const auto& desc : attributeDescriptions) {
        stride += desc.size_;
    }
    std::cerr << stride << std::endl;
    glBufferData(GL_ARRAY_BUFFER, sizeOfVertexData, vertexData, GL_STATIC_DRAW);
    for (int index = 0, relativeOffset = 0; index < attributeDescriptions.size(); index++) {
        const auto& desc = attributeDescriptions[index];
        glEnableVertexAttribArray(index);
        if (desc.type_ == GL_UNSIGNED_SHORT && !desc.normalized_) {
            glVertexAttribIPointer(index, desc.numOfChannel_, desc.type_, stride, reinterpret_cast<const void*>(relativeOffset));
        } else {
            glVertexAttribPointer(index, desc.numOfChannel_, desc.type_, desc.normalized_, stride, reinterpret_cast<const void*>(relativeOffset));
        }
        relativeOffset += desc.size_;
    }
    
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeOfIndexData, indexData, GL_STATIC_DRAW);
    indexType_ = GL_UNSIGNED_INT;
    glBindVertexArray(0);
}
}
