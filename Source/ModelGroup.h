//
//  Mesh.hpp
//  JustRay
//
//  Created by xin on 2017/6/17.
//
//

#ifndef Mesh_h
#define Mesh_h
#include "Vertex.h"
#include "Material.h"
#include "Platform.h"
#include <string>
#include <vector>
#include <tuple>
#include <memory>
namespace JustRay
{
class ModelGroup
{
friend class RenderEngine;
public:
    ModelGroup(const std::string& name);
    ~ModelGroup();
private:
    void Load(const void* vertexData, const int sizeOfVertexData, const std::vector<VertexAttributeDescription>& attributeDescriptions, const void* indexData, const int sizeOfIndexData);
private:
    GLuint vertexArrayID_;
    GLuint vertexBufferID_;
    GLuint indexBufferID_;
    GLenum indexType_;
    // indexOffset, numOfIndices, material
    std::vector<std::tuple<int,int,std::shared_ptr<Material>>> models_;
};
}
#endif /* Mesh_hpp */
