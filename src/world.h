//
//

#ifndef BLOCKY_WORLD_H
#define BLOCKY_WORLD_H

#include <map>
#include <string>

#include <geek/core-logger.h>

#include "chunk.h"
#include "perlin.h"

#define CHUNK_NUM(_i) ((_i >= 0) ? (_i / 16) : (((_i + 1) /16) - 1))

class World : private Geek::Logger
{
 private:
    std::map<std::string, Chunk*> m_chunks;
    PerlinNoise* m_perlin = nullptr;

 public:
    World();
    ~World();

    void generate();

    Chunk* getChunk(int x, int z);
    Chunk* getChunkFromBlock(int x, int y, int z);

    void setBlock(int x, int y, int z, Block* block);
    Block* getBlock(int x, int y, int z);
};

#endif //BLOCKY_WORLD_H
