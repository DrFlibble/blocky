//
// Created by Ian Parker on 12/12/2022.
//

#ifndef BLOCKY_TERRAINCHUNK_H
#define BLOCKY_TERRAINCHUNK_H

#include <geek/core-maths.h>
#include "brick/engine.h"

class Terrain;

class TerrainChunk : Geek::Logger
{
 private:
    Terrain* m_terrain;

    int m_sizeX;
    int m_sizeZ;
    int m_offsetX = 1024;
    int m_offsetZ = 1024;
    float m_scale = 1.0;

    int m_minPixel = 1000;
    int m_pixelRange = 0;

    Geek::Gfx::Surface* m_heightMap;

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;
    std::vector<float> grassVertices;

    GLuint m_vbo = 0;
    GLuint m_ibo = 0;
    GLuint m_vao = 0;

    GLuint m_grassVBO = 0;
    GLuint m_grassVAO = 0;

    Geek::Vector getVertex(unsigned int idx)
    {
        Geek::Vector v;
        v.x = vertices[(idx * 3) + 0];
        v.y = vertices[(idx * 3) + 1];
        v.z = vertices[(idx * 3) + 2];
        return v;
    }
    void addTriangle(
        unsigned int x0,
        unsigned int z0,
        unsigned int x1,
        unsigned int z1,
        unsigned int x2,
        unsigned int z2);

 public:
    TerrainChunk(Terrain* terrain, int sizeX, int sizeZ);

    bool init();
    void bind() const;

    void draw() const;

    float heightAt(float x, float z);
    float pixelAt(float x, float z);
};

#endif //BLOCKY_TERRAINCHUNK_H
