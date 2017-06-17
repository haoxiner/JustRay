//
//  Vertex.cpp
//  JustRay
//
//  Created by xin on 2017/6/17.
//
//

#include "Vertex.h"
namespace JustRay
{
bool Vertex::operator<(const Vertex& rhs) const
{
    return std::memcmp((void*)this, (void*)&rhs, sizeof(Vertex)) < 0;
};
};
