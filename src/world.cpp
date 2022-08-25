//
//

#include "world.h"

using namespace std;
using namespace Geek;


World::World() : Logger("World")
{
    m_perlin = new PerlinNoise();
}

World::~World() = default;

Chunk* World::getChunk(int x, int z)
{
    char chunkId[1024];
    sprintf(chunkId, "%d:0:%d", x, z);

    Chunk* chunk;
    auto it = m_chunks.find(chunkId);
    if (it != m_chunks.end())
    {
        chunk = it->second;
    }
    else
    {
        log(DEBUG, "getChunkFromBlock: Creating chunk: %s", chunkId);
        chunk = new Chunk(this, x, z);
        chunk->generate(m_perlin);
        m_chunks.insert(make_pair(chunkId, chunk));
    }

    return chunk;
}

Chunk* World::getChunkFromBlock(int x, int y, int z)
{
    int chunkX = CHUNK_NUM(x);
    int chunkZ = CHUNK_NUM(z);

    return getChunk(chunkX, chunkZ);
}

void World::setBlock(int x, int y, int z, Block* block, bool updateVisibility)
{
    Chunk* chunk = getChunkFromBlock(x, y, z);
    chunk->setBlock(CHUNK_POS(x), y , CHUNK_POS(z), block);
    if (updateVisibility)
    {
        chunk->updateVisibility();
    }
}

Block* World::getBlock(int x, int y, int z)
{
    Chunk* chunk = getChunkFromBlock(x, y, z);
    return chunk->getBlock(CHUNK_POS(x), y, CHUNK_POS(z));
}

void World::setBlock(Vector &pos, Block* block, bool updateVisibility)
{
    setBlock(floor(pos.x), floor(pos.y), floor(pos.z), block, updateVisibility);
}

Block* World::getBlock(Vector &pos)
{
    return getBlock(floor(pos.x), floor(pos.y), floor(pos.z));
}

