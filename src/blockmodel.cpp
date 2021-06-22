//
//

#include "blockmodel.h"

// unit cube //////////////////////////////////////////////////////////////////
//    v6----- v5
//   /|      /|
//  v1------v0|
//  | |     | |
//  | v7----|-v4
//  |/      |/
//  v2------v3

// vertex coords array for glDrawElements() ===================================
// A cube has 6 sides and each side has 4 vertices, therefore, the total number
// of vertices is 24 (6 sides * 4 verts), and 72 floats in the vertex array
// since each vertex has 3 components (x,y,z) (= 24 * 3)
static const GLfloat vertices[] = {
    .5f, .5f, .5f, -.5f, .5f, .5f, -.5f, -.5f, .5f, .5f, -.5f, .5f,   // v0,v1,v2,v3 (front)
    .5f, .5f, .5f, .5f, -.5f, .5f, .5f, -.5f, -.5f, .5f, .5f, -.5f,   // v0,v3,v4,v5 (right)
    .5f, .5f, .5f, .5f, .5f, -.5f, -.5f, .5f, -.5f, -.5f, .5f, .5f,   // v0,v5,v6,v1 (top)
    -.5f, .5f, .5f, -.5f, .5f, -.5f, -.5f, -.5f, -.5f, -.5f, -.5f, .5f,   // v1,v6,v7,v2 (left)
    -.5f, -.5f, -.5f, .5f, -.5f, -.5f, .5f, -.5f, .5f, -.5f, -.5f, .5f,   // v7,v4,v3,v2 (bottom)
    .5f, -.5f, -.5f, -.5f, -.5f, -.5f, -.5f, .5f, -.5f, .5f, .5f, -.5f    // v4,v7,v6,v5 (back)
};

// normal array
static const GLfloat normals[] = {
    0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,  // v0,v1,v2,v3 (front)
    1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,  // v0,v3,v4,v5 (right)
    0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,  // v0,v5,v6,v1 (top)
    -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0,  // v1,v6,v7,v2 (left)
    0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0,  // v7,v4,v3,v2 (bottom)
    0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1   // v4,v7,v6,v5 (back)
};

// color array
static const GLfloat colors[] = {
    1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1,  // v0,v1,v2,v3 (front)
    1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1,  // v0,v3,v4,v5 (right)
    1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0,  // v0,v5,v6,v1 (top)
    1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0,  // v1,v6,v7,v2 (left)
    0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0,  // v7,v4,v3,v2 (bottom)
    0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1   // v4,v7,v6,v5 (back)
};

// texCoord array
static const GLfloat texCoords[] = {
    1, 0, 0, 0, 0, 1, 1, 1,               // v0,v1,v2,v3 (front)
    0, 0, 0, 1, 1, 1, 1, 0,               // v0,v3,v4,v5 (right)
    1, 1, 1, 0, 0, 0, 0, 1,               // v0,v5,v6,v1 (top)
    1, 0, 0, 0, 0, 1, 1, 1,               // v1,v6,v7,v2 (left)
    0, 1, 1, 1, 1, 0, 0, 0,               // v7,v4,v3,v2 (bottom)
    0, 1, 1, 1, 1, 0, 0, 0                // v4,v7,v6,v5 (back)
};

// index array for glDrawElements() ===========================================
// A cube has 36 indices = 6 sides * 2 tris * 3 verts
const GLuint indices[] = {
    0, 1, 2, 2, 3, 0,    // v0-v1-v2, v2-v3-v0 (front)
    4, 5, 6, 6, 7, 4,    // v0-v3-v4, v4-v5-v0 (right)
    8, 9, 10, 10, 11, 8,    // v0-v5-v6, v6-v1-v0 (top)
    12, 13, 14, 14, 15, 12,    // v1-v6-v7, v7-v2-v1 (left)
    16, 17, 18, 18, 19, 16,    // v7-v4-v3, v3-v2-v7 (bottom)
    20, 21, 22, 22, 23, 20     // v4-v7-v6, v6-v5-v4 (back)
};

BlockModel::BlockModel() = default;

BlockModel::~BlockModel() = default;

bool BlockModel::init()
{
    glGenBuffers(1, &m_cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(normals) + sizeof(colors) + sizeof(texCoords), nullptr, GL_STATIC_DRAW);
    // copy vertices starting from 0 offset
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    // copy normals after vertices
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(normals), normals);
    // copy colours after normals
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(normals), sizeof(colors), colors);
    // copy texture coords after colours
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(normals) + sizeof(colors), sizeof(texCoords), texCoords);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_cubeIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return true;
}

void BlockModel::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeIBO);
}

void* BlockModel::getPositionPtr() const
{
    return nullptr;
}

void* BlockModel::getNormalPtr() const
{
    return (void*)sizeof(vertices);
}

void* BlockModel::getTexCoordPtr() const
{
    return (void*)(sizeof(vertices)+sizeof(normals)+sizeof(colors));
}


