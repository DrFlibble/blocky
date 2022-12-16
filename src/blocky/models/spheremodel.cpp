//
// Created by Ian Parker on 23/09/2022.
//

#include "spheremodel.h"
#include "brick/engine.h"

using namespace std;

bool SphereModel::init()
{
    // tmp vertex definition (x,y,z,s,t)
    struct Vertex
    {
        float x, y, z, s, t;
    };
    std::vector<Vertex> tmpVertices;

    float sectorStep = 2.0f * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    // compute all vertices first, each vertex contains (x,y,z,s,t) except normal
    for(int i = 0; i <= stackCount; ++i)
    {
        stackAngle = M_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        float xy = radius * cosf(stackAngle);       // r * cos(u)
        float z = radius * sinf(stackAngle);        // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for(int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            Vertex vertex;
            vertex.x = xy * cosf(sectorAngle);      // x = r * cos(u) * cos(v)
            vertex.y = xy * sinf(sectorAngle);      // y = r * cos(u) * sin(v)
            vertex.z = z;                           // z = r * sin(u)
            vertex.s = (float)j/sectorCount;        // s
            vertex.t = (float)i/stackCount;         // t
            tmpVertices.push_back(vertex);
        }
    }

    Vertex v1, v2, v3, v4;                          // 4 vertex positions and tex coords
    std::vector<float> n;                           // 1 face normal

    int i, j, k, vi1, vi2;
    int index = 0;                                  // index for vertex
    for(i = 0; i < stackCount; ++i)
    {
        vi1 = i * (sectorCount + 1);                // index of tmpVertices
        vi2 = (i + 1) * (sectorCount + 1);

        for(j = 0; j < sectorCount; ++j, ++vi1, ++vi2)
        {
            // get 4 vertices per sector
            //  v1--v3
            //  |    |
            //  v2--v4
            v1 = tmpVertices[vi1];
            v2 = tmpVertices[vi2];
            v3 = tmpVertices[vi1 + 1];
            v4 = tmpVertices[vi2 + 1];

            // if 1st stack and last stack, store only 1 triangle per sector
            // otherwise, store 2 triangles (quad) per sector
            if(i == 0) // a triangle for first stack ==========================
            {
                // put a triangle
                addVertex(v1.x, v1.y, v1.z);
                addVertex(v2.x, v2.y, v2.z);
                addVertex(v4.x, v4.y, v4.z);

                // put tex coords of triangle
                addTexCoord(v1.s, v1.t);
                addTexCoord(v2.s, v2.t);
                addTexCoord(v4.s, v4.t);

                // put normal
                n = computeFaceNormal(v1.x,v1.y,v1.z, v2.x,v2.y,v2.z, v4.x,v4.y,v4.z);
                for(k = 0; k < 3; ++k)  // same normals for 3 vertices
                {
                    addNormal(n[0], n[1], n[2]);
                }

                // put indices of 1 triangle
                addIndices(index, index+1, index+2);

                // indices for line (first stack requires only vertical line)
                //lineIndices.push_back(index);
                //lineIndices.push_back(index+1);

                index += 3;     // for next
            }
            else if(i == (stackCount-1)) // a triangle for last stack =========
            {
                // put a triangle
                addVertex(v1.x, v1.y, v1.z);
                addVertex(v2.x, v2.y, v2.z);
                addVertex(v3.x, v3.y, v3.z);

                // put tex coords of triangle
                addTexCoord(v1.s, v1.t);
                addTexCoord(v2.s, v2.t);
                addTexCoord(v3.s, v3.t);

                // put normal
                n = computeFaceNormal(v1.x,v1.y,v1.z, v2.x,v2.y,v2.z, v3.x,v3.y,v3.z);
                for(k = 0; k < 3; ++k)  // same normals for 3 vertices
                {
                    addNormal(n[0], n[1], n[2]);
                }

                // put indices of 1 triangle
                addIndices(index, index+1, index+2);

                // indices for lines (last stack requires both vert/hori lines)
                //lineIndices.push_back(index);
                //lineIndices.push_back(index+1);
                //lineIndices.push_back(index);
                //lineIndices.push_back(index+2);

                index += 3;     // for next
            }
            else // 2 triangles for others ====================================
            {
                // put quad vertices: v1-v2-v3-v4
                addVertex(v1.x, v1.y, v1.z);
                addVertex(v2.x, v2.y, v2.z);
                addVertex(v3.x, v3.y, v3.z);
                addVertex(v4.x, v4.y, v4.z);

                // put tex coords of quad
                addTexCoord(v1.s, v1.t);
                addTexCoord(v2.s, v2.t);
                addTexCoord(v3.s, v3.t);
                addTexCoord(v4.s, v4.t);

                // put normal
                n = computeFaceNormal(v1.x,v1.y,v1.z, v2.x,v2.y,v2.z, v3.x,v3.y,v3.z);
                for(k = 0; k < 4; ++k)  // same normals for 4 vertices
                {
                    addNormal(n[0], n[1], n[2]);
                }

                // put indices of quad (2 triangles)
                addIndices(index, index+1, index+2);
                addIndices(index+2, index+1, index+3);

                // indices for lines
                //lineIndices.push_back(index);
                //lineIndices.push_back(index+1);
                //lineIndices.push_back(index);
                //lineIndices.push_back(index+2);

                index += 4;     // for next
            }
        }
    }

    GL(glGenBuffers(1, &m_vbo));
    GL(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));

    uintptr_t verticesSize = vertices.size() * sizeof(float);
    uintptr_t normalsSize = normals.size() * sizeof(float);
    uintptr_t texCoordsSize = texCoords.size() * sizeof(float);
    uintptr_t indicesSize = indices.size() * sizeof(unsigned int);

    GL(glBufferData(GL_ARRAY_BUFFER, verticesSize + normalsSize + texCoordsSize, nullptr, GL_STATIC_DRAW));
    GL(glBufferSubData(GL_ARRAY_BUFFER, 0, verticesSize, vertices.data()));
    GL(glBufferSubData(GL_ARRAY_BUFFER, verticesSize, normalsSize, normals.data()));
    GL(glBufferSubData(GL_ARRAY_BUFFER, verticesSize + normalsSize, texCoordsSize, texCoords.data()));

    GL(glGenBuffers(1, &m_ibo));
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo));
    GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices.data(), GL_STATIC_DRAW));

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    GL(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo));

    glBindVertexArray(0);
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    return false;
}

void SphereModel::bind() const
{
    glBindVertexArray(m_vao);
    GL(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo));
}

Side SphereModel::hit(const Geek::Vector &pos, Ray* ray)
{
    return X_POS;
}

void* SphereModel::getPositionPtr() const
{
    return nullptr;
}

void* SphereModel::getNormalPtr() const
{
    return nullptr;
}

void* SphereModel::getTexCoordPtr() const
{
    return nullptr;
}

void SphereModel::addVertex(float x, float y, float z)
{
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
}

void SphereModel::addNormal(float nx, float ny, float nz)
{
    normals.push_back(nx);
    normals.push_back(ny);
    normals.push_back(nz);
}

void SphereModel::addTexCoord(float s, float t)
{
    texCoords.push_back(s);
    texCoords.push_back(t);
}

void SphereModel::addIndices(unsigned int i1, unsigned int i2, unsigned int i3)
{
    indices.push_back(i1);
    indices.push_back(i2);
    indices.push_back(i3);
}

std::vector<float> SphereModel::computeFaceNormal(float x1, float y1, float z1,  // v1
                                             float x2, float y2, float z2,  // v2
                                             float x3, float y3, float z3)  // v3
{
    const float EPSILON = 0.000001f;

    std::vector<float> normal(3, 0.0f);     // default return value (0,0,0)
    float nx, ny, nz;

    // find 2 edge vectors: v1-v2, v1-v3
    float ex1 = x2 - x1;
    float ey1 = y2 - y1;
    float ez1 = z2 - z1;
    float ex2 = x3 - x1;
    float ey2 = y3 - y1;
    float ez2 = z3 - z1;

    // cross product: e1 x e2
    nx = ey1 * ez2 - ez1 * ey2;
    ny = ez1 * ex2 - ex1 * ez2;
    nz = ex1 * ey2 - ey1 * ex2;

    // normalize only if the length is > 0
    float length = sqrtf(nx * nx + ny * ny + nz * nz);
    if(length > EPSILON)
    {
        // normalize
        float lengthInv = 1.0f / length;
        normal[0] = nx * lengthInv;
        normal[1] = ny * lengthInv;
        normal[2] = nz * lengthInv;
    }

    return normal;
}

void SphereModel::draw() const
{
    // activate attribs
    GL(glEnableVertexAttribArray(0));
    GL(glEnableVertexAttribArray(1));
    GL(glEnableVertexAttribArray(2));

    uint64_t verticesSize = vertices.size() * sizeof(float);
    uint64_t normalsSize = normals.size() * sizeof(float);
    GL(glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0));
    GL(glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, (void*)verticesSize));
    GL(glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, (void*)(verticesSize + normalsSize)));

    GL(glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, nullptr));

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);//attribVertexNormal);
    glDisableVertexAttribArray(2);//attribVertexTexCoord);

}
