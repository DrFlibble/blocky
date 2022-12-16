//
// Created by Ian Parker on 14/12/2022.
//

#ifndef BLOCKY_TERRAIN_H
#define BLOCKY_TERRAIN_H

#include "terrainchunk.h"

class BottEngine;

class Terrain
{
 private:
    BottEngine* m_engine = nullptr;
    TerrainChunk* m_chunk = nullptr;
    Geek::Gfx::Surface* m_terrainSurface = nullptr;
    Texture* m_terrainTexture = nullptr;

 public:
    Terrain(BottEngine* engine) : m_engine(engine) {}

    bool init();
    bool bind();
    bool draw();

    [[nodiscard]] BottEngine* getEngine() const { return m_engine; }
    [[nodiscard]] Geek::Gfx::Surface* getSurface() const { return m_terrainSurface; }
    [[nodiscard]] Texture* getTexture() const { return m_terrainTexture; }

    float heightAt(float x, float z) { return m_chunk->heightAt(x, z); }
};


#endif //BLOCKY_TERRAIN_H
