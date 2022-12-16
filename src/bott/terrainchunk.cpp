//
// Created by Ian Parker on 12/12/2022.
//

#include "terrainchunk.h"
#include "terrain.h"
#include "bott.h"
#include "grassshader.h"
#include <brick/texture.h>
#include <geek/gfx-surface.h>
#include <geek/core-random.h>

#include <stdlib.h>

using namespace std;
using namespace Geek;
using namespace Geek::Core;
using namespace Geek::Gfx;

TerrainChunk::TerrainChunk(Terrain* terrain, int sizeX, int sizeZ) :
    Logger("TerrainChunk"),
    m_terrain(terrain),
    m_sizeX(sizeX),
    m_sizeZ(sizeZ)
{
}

bool TerrainChunk::init()
{
    //Surface* heightMap = Surface::loadPNG("../data/bott/textures/MountainRangeHeightMap.png");
    m_heightMap = Surface::loadPNG("../data/bott/textures/RockyLandAndRiversHeightMap.png");

    int mapWidth = m_heightMap->getWidth();
    int mapHeight = m_heightMap->getHeight();

    int maxPixel = -1000;
    for (int y = 0; y < mapHeight; y++)
    {
        for (int x = 0; x < mapWidth; x++)
        {
            int p = m_heightMap->getPixel(x, y) & 0xff;
            if (p < m_minPixel)
            {
                m_minPixel = p;
            }
            if (p > maxPixel)
            {
                maxPixel = p;
            }
        }
    }
    m_pixelRange = maxPixel - m_minPixel;
    printf("\nHeight map: minP=%d, maxP=%d, pixelRange=%d\n", m_minPixel, maxPixel, m_pixelRange);

    float min = 1000;
    float max = -1000;
    vertices.reserve(m_sizeX * m_sizeZ * 3);
    Random* random = new Random();
    for (int z = 0; z < m_sizeZ; z++)
    {
        for (int x = 0; x < m_sizeX; x++)
        {
            //vertices.push_back(((float)x / (float)(m_sizeX - 1)) * (float)m_sizeX);
            //vertices.push_back(0);
            //vertices.push_back(((float)z / (float)(m_sizeZ - 1)) * (float)m_sizeZ);
            float y = heightAt(x * m_scale, z * m_scale);
            if (y < min)
            {
                min = y;
            }
            if (y > max)
            {
                max = y;
            }

            vertices.push_back((float) x * m_scale);
            vertices.push_back(y);
            vertices.push_back((float) z * m_scale);
            uint32_t p = pixelAt(x, z);
            uint8_t r = p & 0xff;
            uint8_t g = (p >> 8) & 0xff;
            uint8_t b = (p >> 16) & 0xff;

            float tx = 0.1;
            if ((x % 2) == 1)
            {
                tx = 0.49;
            }
            float ty = 0.1;
            if ((z % 2) == 1)
            {
                ty = 0.49;
            }

            if (g > 10 && g > r * 2 && g > b * 2)
            {
                for (float gz = -0.5; gz < 0.5; gz += 0.1)
                {
                    for (float gx = -0.5; gx < 0.5; gx += 0.1)
                    {
                        // Grass?
                        float grassX = ((float) x + gx + (float) random->ranged(-0.25, 0.25)) * m_scale;
                        float grassZ = ((float) z + gz + (float) random->ranged(-0.25, 0.25)) * m_scale;
                        grassVertices.push_back(grassX);
                        grassVertices.push_back(y);
                        grassVertices.push_back(grassZ);
                    }
                }
                //texCoords.push_back((float) (x + m_offsetX) / (float) (mapWidth));
                //texCoords.push_back((float) (z + m_offsetZ) / (float) (mapHeight));
                texCoords.push_back(tx);
                texCoords.push_back(ty);
            }
            else
            {
                //texCoords.push_back((float) (x + m_offsetX) / (float) (mapWidth));
                //texCoords.push_back((float) (z + m_offsetZ) / (float) (mapHeight));
                texCoords.push_back(tx + 0.5);
                texCoords.push_back(ty);
            }
        }
        //printf("\n");
    }
    printf("Height: min=%0.2f, max=%0.2f\n", min, max);
    printf("Grass: %lu\n", grassVertices.size() / 3);

    indices.reserve(2 * (m_sizeX - 1) * (m_sizeZ - 1) * 3);

    for (size_t z = 0; z < m_sizeZ - 1; z++)
    {
        for (size_t x = 0; x < m_sizeX - 1; x++)
        {
                addTriangle(
                    x + 1, z,
                    x, z,
                    x, z + 1);
                addTriangle(
                    x + 1, z + 1,
                    x + 1, z,
                    x, z + 1);
                //printf("\n");
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

    // Grass
    glGenVertexArrays(1, &m_grassVAO);
    glGenBuffers(1, &m_grassVBO);
    glBindVertexArray(m_grassVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_grassVBO);
    glBufferData(GL_ARRAY_BUFFER, grassVertices.size() * sizeof(float), grassVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindVertexArray(0);
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    return true;
}

void TerrainChunk::addTriangle(
    unsigned int x0,
    unsigned int z0,
    unsigned int x1,
    unsigned int z1,
    unsigned int x2,
    unsigned int z2)
{
    unsigned int p0 = x0 + (z0 * m_sizeX);
    unsigned int p1 = x1 + (z1 * m_sizeX);
    unsigned int p2 = x2 + (z2 * m_sizeX);

    indices.push_back(p0);
    indices.push_back(p1);
    indices.push_back(p2);

    Geek::Vector v0 = getVertex(p0);
    Geek::Vector v1 = getVertex(p1);
    Geek::Vector v2 = getVertex(p2);
    Geek::Vector normal = (v1 - v0).cross(v2 - v0).normalise();
    normals.push_back(normal.x);
    normals.push_back(normal.y);
    normals.push_back(normal.z);
}

void TerrainChunk::bind() const
{
    glBindVertexArray(m_vao);
    GL(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo));
}

void TerrainChunk::draw() const
{
    glActiveTexture(GL_TEXTURE0);
    m_terrain->getTexture()->bind();

    GL(glEnableVertexAttribArray(0));
    GL(glEnableVertexAttribArray(1));
    GL(glEnableVertexAttribArray(2));

    uint64_t verticesSize = vertices.size() * sizeof(float);
    uint64_t normalsSize = normals.size() * sizeof(float);
    GL(glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0));
    GL(glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, (void*) verticesSize));
    GL(glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, (void*) (verticesSize + normalsSize)));

    GL(glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, nullptr));

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);//attribVertexNormal);
    glDisableVertexAttribArray(2);//attribVertexTexCoord);

    // Draw grass
    m_terrain->getEngine()->getGrassShader()->use();

    GL(glEnableVertexAttribArray(0));
    glActiveTexture(GL_TEXTURE1);
    m_terrain->getEngine()->getGrassFlowTexture()->bind();
    glActiveTexture(GL_TEXTURE0);
    m_terrain->getEngine()->getGrassTexture()->bind();
    glBindVertexArray(m_grassVAO);
    //GL(glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0));
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glDrawArrays(GL_POINTS, 0, grassVertices.size());

    m_terrain->getEngine()->getGrassShader()->unuse();
    //glDisableVertexAttribArray(0);
}

float TerrainChunk::heightAt(float x, float z)
{
    uint32_t p = m_heightMap->getPixel((x + m_offsetX) / m_scale, (z + m_offsetZ) / m_scale);
    p = (p & 0xff) - m_minPixel;
    return ((float) (p & 0xff) / m_pixelRange) * 50.0;
}

float TerrainChunk::pixelAt(float x, float z)
{
    return m_terrain->getSurface()->getPixel(
        (x + m_offsetX) / m_scale,
        (z + m_offsetZ) / m_scale);
}
