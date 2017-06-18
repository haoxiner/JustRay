//
//  VertexBuffer.cpp
//  JustRay
//
//  Created by xin on 2017/6/17.
//
//

#include "VertexBuffer.h"
#include "Platform.h"
namespace JustRay
{
VertexBuffer::VertexBuffer(const void* vertexData, const int sizeOfVertexData, const std::vector<VertexAttributeDescription>& attributeDescriptions, const void* indexData, const int sizeOfIndexData)
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
VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &indexBufferID_);
    glDeleteBuffers(1, &vertexBufferID_);
    glDeleteVertexArrays(1, &vertexArrayID_);
}
};
