//
//

#include "world.h"

using namespace std;
using namespace Geek;

#define CHUNK_POS(_i) ((_i >= 0) ? (_i % 16) : (((_i + 1) % 16) + 15))

World::World() : Logger("World")
{
    m_perlin = new PerlinNoise();
}

World::~World()
{

}

void World::generate()
{
    for (int x = -100; x < 100; x++)
    {
        for (int z = -100; z < 100; z++)
        {
            float n = m_perlin->noise((float)x * 0.5, (float)z * 0.5) + 0.5;
// log(DEBUG, "noise: %d, %d -> %0.2f", x, z, n);
            int y = n * 8;
            if (y < 0)
            {
                y = 0;
            }
            setBlock(x, y, z, new Block(GRASS));
            y--;
            for (; y > 0; y--)
            {
                setBlock(x, y, z, new Block(DIRT));
            }
        }
    }

    for (int y = 1; y < 16; y++)
    {
        setBlock(5, y, 5, new Block(DIRT));
    }

}

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
        chunk = new Chunk(x, z);
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

void World::setBlock(int x, int y, int z, Block* block)
{
    Chunk* chunk = getChunkFromBlock(x, y, z);
    chunk->setBlock(CHUNK_POS(x), y , CHUNK_POS(z), block);
}

Block* World::getBlock(int x, int y, int z)
{
    Chunk* chunk = getChunkFromBlock(x, y, z);
    return chunk->getBlock(CHUNK_POS(x), y, CHUNK_POS(z));
}

