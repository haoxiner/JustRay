//
//  Vertex.h
//  JustRay
//
//  Created by xin on 2017/6/17.
//
//

#pragma once
#include "MathUtil.h"
#include "Platform.h"
#include <memory>
namespace JustRay
{
struct Vertex
{
    Float3 position;
    Int_2_10_10_10 normal;
    Float2 texcoord;
    bool operator<(const Vertex& rhs) const;
};
struct VertexAttributeDescription
{
    int numOfChannel_;
    GLenum type_;
    bool normalized_;
    int size_;
    VertexAttributeDescription(int numOfChannel,
                      GLenum type,
                      bool normalized,
                      int size) :
        numOfChannel_(numOfChannel), type_(type), normalized_(normalized), size_(size)
    {
    }
};
}
