//
//  SkyBox.cpp
//  JustRay
//
//  Created by xin on 2017/7/3.
//
//

#include "SkyBox.h"
#include "MathUtil.h"
#include <vector>
namespace JustRay
{
SkyBox::SkyBox()
{
    std::vector<short> positions = {
        // bottom
        MapToShort(-1.0f), MapToShort(1.0f), MapToShort(-1.0f), MapToShort(0.0f), // top left 0
        MapToShort(1.0f),  MapToShort(1.0f), MapToShort(-1.0f), MapToShort(0.0f), // top right 1
        MapToShort(-1.0f), MapToShort(-1.0f),  MapToShort(-1.0f), MapToShort(0.0f), // bottom left 2
        MapToShort(1.0f),  MapToShort(-1.0f),  MapToShort(-1.0f), MapToShort(0.0f), // bottom right 3
        // top
        MapToShort(-1.0f), MapToShort(1.0f), MapToShort(1.0f), MapToShort(0.0f), // top left 4
        MapToShort(1.0f),  MapToShort(1.0f), MapToShort(1.0f), MapToShort(0.0f), // top right 5
        MapToShort(-1.0f), MapToShort(-1.0f),  MapToShort(1.0f), MapToShort(0.0f), // bottom left 6
        MapToShort(1.0f),  MapToShort(-1.0f),  MapToShort(1.0f), MapToShort(0.0f) // bottom right 7
    };
    std::vector<unsigned short> indices = {
        0,2,1, 1,2,3, // bottom
        6,4,7, 7,4,5, // top
        6,2,4, 4,2,0, // left
        5,1,7, 7,1,3, // right
        4,0,5, 5,0,1, // front
        6,7,2, 7,3,2 // back
    };
    glGenVertexArrays(1, &vertexArrayID_);
    glBindVertexArray(vertexArrayID_);
    glGenBuffers(1, &vertexBufferID_);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_SHORT, GL_TRUE, 0, reinterpret_cast<GLvoid*>(0));
    glGenBuffers(1, &indexBufferID_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
}
void SkyBox::Render()
{
    glBindVertexArray(vertexArrayID_);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid*>(0));
}
}
