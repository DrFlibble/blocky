//
//

#ifndef BLOCKY_WORLD_H
#define BLOCKY_WORLD_H

#include <map>
#include <string>

#include <geek/core-logger.h>
#include <geek/core-maths.h>

#include "chunk.h"
#include "perlin.h"
#include "player.h"

class Chunk;
class Block;

#define CHUNK_NUM(_i) ((_i >= 0) ? (_i / 16) : (((_i + 1) /16) - 1))
#define CHUNK_POS(_i) ((_i >= 0) ? (_i % 16) : (((_i + 1) % 16) + 15))

class World : private Geek::Logger
{
 private:
    std::map<std::string, Chunk*> m_chunks;
    PerlinNoise* m_perlin = nullptr;

    Player m_player;

 public:
    World();
    ~World();

    //void generate();

    Player& getPlayer() { return m_player; }

    Chunk* getChunk(int x, int z);
    Chunk* getChunkFromBlock(int x, int y, int z);

    void setBlock(Geek::Vector& pos, Block* block, bool updateVisibility = false);
    void setBlock(int x, int y, int z, Block* block, bool updateVisibility = false);
    Block* getBlock(int x, int y, int z);
    Block* getBlock(Geek::Vector& pos);

    void save();
};

#endif //BLOCKY_WORLD_H
