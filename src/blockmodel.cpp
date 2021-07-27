//
//

#include "blockmodel.h"
#include "blocky.h"
#include "ray.h"

const double epsilon = std::numeric_limits<double>::epsilon();

using namespace std;
using namespace Geek;

// unit cube //////////////////////////////////////////////////////////////////
//    v6----- v5
//   /|      /|
//  v1------v0|
//  | |     | |
//  | v7----|-v4
//  |/      |/
//  v2------v3

// vertex coords array for glDrawElements() ===================================
// A cube has 6 sides and each side has 4 g_cubeVertices, therefore, the total number
// of g_cubeVertices is 24 (6 sides * 4 verts), and 72 floats in the vertex array
// since each vertex has 3 components (x,y,z) (= 24 * 3)
static const GLfloat g_cubeVertices[] = {
    .5f, .5f, .5f, -.5f, .5f, .5f, -.5f, -.5f, .5f, .5f, -.5f, .5f,   // v0,v1,v2,v3 (front)
    .5f, .5f, .5f, .5f, -.5f, .5f, .5f, -.5f, -.5f, .5f, .5f, -.5f,   // v0,v3,v4,v5 (right)
    .5f, .5f, .5f, .5f, .5f, -.5f, -.5f, .5f, -.5f, -.5f, .5f, .5f,   // v0,v5,v6,v1 (top)
    -.5f, .5f, .5f, -.5f, .5f, -.5f, -.5f, -.5f, -.5f, -.5f, -.5f, .5f,   // v1,v6,v7,v2 (left)
    -.5f, -.5f, -.5f, .5f, -.5f, -.5f, .5f, -.5f, .5f, -.5f, -.5f, .5f,   // v7,v4,v3,v2 (bottom)
    .5f, -.5f, -.5f, -.5f, -.5f, -.5f, -.5f, .5f, -.5f, .5f, .5f, -.5f    // v4,v7,v6,v5 (back)
};

// normal array
static const GLfloat g_cubeNormals[] = {
    0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,  // v0,v1,v2,v3 (front)
    1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,  // v0,v3,v4,v5 (right)
    0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,  // v0,v5,v6,v1 (top)
    -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0,  // v1,v6,v7,v2 (left)
    0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0,  // v7,v4,v3,v2 (bottom)
    0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1   // v4,v7,v6,v5 (back)
};

// texCoord array
static const GLfloat g_cubeTexCoords[] = {
    1, 0, 0, 0, 0, 1, 1, 1,               // v0,v1,v2,v3 (front)
    0, 0, 0, 1, 1, 1, 1, 0,               // v0,v3,v4,v5 (right)
    1, 1, 1, 0, 0, 0, 0, 1,               // v0,v5,v6,v1 (top)
    1, 0, 0, 0, 0, 1, 1, 1,               // v1,v6,v7,v2 (left)
    0, 1, 1, 1, 1, 0, 0, 0,               // v7,v4,v3,v2 (bottom)
    0, 1, 1, 1, 1, 0, 0, 0                // v4,v7,v6,v5 (back)
};

// index array for glDrawElements() ===========================================
// A cube has 36 g_cubeIndices = 6 sides * 2 tris * 3 verts
const int g_cubeIndices[] = {
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
    GL(glGenBuffers(1, &m_cubeVBO));
    GL(glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO));
    GL(glBufferData(GL_ARRAY_BUFFER, sizeof(g_cubeVertices) + sizeof(g_cubeNormals) + sizeof(g_cubeTexCoords), nullptr, GL_STATIC_DRAW));
    // copy g_cubeVertices starting from 0 offset
    GL(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(g_cubeVertices), g_cubeVertices));
    // copy g_cubeNormals after g_cubeVertices
    GL(glBufferSubData(GL_ARRAY_BUFFER, sizeof(g_cubeVertices), sizeof(g_cubeNormals), g_cubeNormals));
    // copy texture coords after g_cubeNormals
    GL(glBufferSubData(GL_ARRAY_BUFFER, sizeof(g_cubeVertices) + sizeof(g_cubeNormals), sizeof(g_cubeTexCoords), g_cubeTexCoords));
    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));

    GL(glGenBuffers(1, &m_cubeIBO));
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeIBO));
    GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_cubeIndices), g_cubeIndices, GL_STATIC_DRAW));
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    glGenVertexArrays(1, &m_cubeVAO);

    glBindVertexArray(m_cubeVAO);
    GL(glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO));
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeIBO));

    return true;
}

void BlockModel::bind() const
{
    glBindVertexArray(m_cubeVAO);
}

void* BlockModel::getPositionPtr() const
{
    return (void*) 0;
}

void* BlockModel::getNormalPtr() const
{
    return (void*) sizeof(g_cubeVertices);
}

void* BlockModel::getTexCoordPtr() const
{
    return (void*) (sizeof(g_cubeVertices) + sizeof(g_cubeNormals));
}

static Vector getVertex(int idx)
{
    return Vector(
        g_cubeVertices[(idx * 3) + 0] + 0.5,
        g_cubeVertices[(idx * 3) + 1] + 0.5,
        g_cubeVertices[(idx * 3) + 2] + 0.5);
}

Side BlockModel::hit(const Geek::Vector &pos, Ray* ray)
{
    Hit closestHit;
    for (int face = 0; face < 12; face++)
    {
        Vector v[3];
        v[0] = getVertex(g_cubeIndices[(face * 3) + 0]);
        v[1] = getVertex(g_cubeIndices[(face * 3) + 1]);
        v[2] = getVertex(g_cubeIndices[(face * 3) + 2]);
        Hit hit;
        if (hitTriangle(v, pos, ray, hit))
        {
            if (hit.t < closestHit.t)
            {
                closestHit = hit;
                closestHit.face = face;
            }
        }
    }
    if (closestHit.face != -1)
    {
        int side = ((closestHit.face) / 2);
        //printf("BlockModel::hit: HIT: hitFace=%d, side=%d\n", closestHit.face, side);
        return (Side)side;
    }
    printf("BlockModel::hit: No hit\n");
    return NONE;
}

bool BlockModel::hitTriangle(Vector* vertices, const Vector &pos, Ray* ray, Hit &hit)
{
    vertices[0] += pos;
    vertices[1] += pos;
    vertices[2] += pos;

    Vector v0v1 = vertices[2] - vertices[0];
    Vector v0v2 = vertices[1] - vertices[0];
    Vector p = ray->direction().cross(v0v2);

    double det = v0v1.dot(p);

    if (abs(det) < epsilon)
    {
        return false;
    }

    double invDet = 1 / det;

    Vector tVec = ray->origin() - vertices[0];

    Vector qVec = tVec.cross(v0v1);

    double t = v0v2.dot(qVec) * invDet;
    if (t < epsilon)// || t > tMax)
    {
        return false;
    }

    double u = tVec.dot(p) * invDet;
    if (u < 0.0 || u > 1.0)
    {
        return false;
    }

    double v = ray->direction().dot(qVec) * invDet;
    if (v < 0.0 || u + v > 1.0)
    {
        return false;
    }

    hit.t = (float)t;

    /*
    hit.position = ray.at(t);
    hit.u = u;
    hit.g_cubeVertices = g_cubeVertices;
    hit.object = this;
    hit.material = getMaterial();
    hit.setFaceNormal(ray, m_normal);
    //hit.normal = ray.direction().cross(m_normal).normalise();
    //hit.normal = m_normal;
    hit.frontFace = true;
     */

    return true;
}


