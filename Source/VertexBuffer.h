//
//  VertexBuffer.h
//  JustRay
//
//  Created by xin on 2017/6/17.
//
//

#pragma once
#include "Vertex.h"
#include "Platform.h"
#include <vector>
namespace JustRay
{
class VertexBuffer
{
friend class RenderEngine;
public:
    VertexBuffer(const void* vertexData, const int sizeOfVertexData, const std::vector<VertexAttributeDescription>& attributeDescriptions, const void* indexData, const int sizeOfIndexData);
    ~VertexBuffer();
private:
    GLuint vertexArrayID_;
    GLuint vertexBufferID_;
    GLuint indexBufferID_;
    GLenum indexType_;
};
}
