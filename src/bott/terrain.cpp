//
// Created by Ian Parker on 14/12/2022.
//

#include "terrain.h"
#include <brick/texture.h>

using namespace Geek::Gfx;

bool Terrain::init()
{
    bool res;

    m_terrainSurface = Surface::loadPNG("../data/bott/textures/RockyLandAndRiversDiffuse.png");
    Surface* terrainSurface = Surface::loadPNG("../data/bott/textures/terrain.png");
    m_terrainTexture = new Texture(terrainSurface);

    m_chunk = new TerrainChunk(this, 128, 128);
    res = m_chunk->init();
    if (!res)
    {
        return false;
    }

    return true;
}

bool Terrain::bind()
{
    m_terrainTexture->bind();
    m_chunk->bind();
    return true;
}

bool Terrain::draw()
{
    m_chunk->draw();
    return true;
}
