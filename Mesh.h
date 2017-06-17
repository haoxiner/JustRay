//
//  Mesh.hpp
//  JustRay
//
//  Created by xin on 2017/6/17.
//
//

#ifndef Mesh_h
#define Mesh_h
#include "Platform.h"
#include <string>
namespace JustRay
{
class Mesh
{
public:
    void LoadMesh(const std::string& name);
private:
    GLuint vao_;
    GLuint vertexBufferID_;
    GLuint indexBufferID_;
};
}
#endif /* Mesh_hpp */
