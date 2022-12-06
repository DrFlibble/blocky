//
//

#ifndef BLOCKY_WORLD_H
#define BLOCKY_WORLD_H

#include <map>
#include <string>
#include <vector>

#include <geek/core-logger.h>
#include <geek/core-maths.h>
#include <geek/core-random.h>

#include "chunk.h"
#include "libbrick/perlin.h"
#include "player.h"
#include "spheremob.h"

class Chunk;
class Block;

#define CHUNK_NUM(_i) ((_i >= 0) ? (_i / 16) : (((_i + 1) /16) - 1))
#define CHUNK_POS(_i) ((_i >= 0) ? (_i % 16) : (((_i + 1) % 16) + 15))

class World : private Geek::Logger
{
 private:
    Geek::Core::Random* m_random;
    uint64_t m_seed;
    std::map<std::string, Chunk*> m_chunks;
    PerlinNoise* m_perlin = nullptr;

    std::vector<SphereMob*> m_mobs;

    Player m_player;

 public:
    World();
    ~World();

    Player& getPlayer() { return m_player; }

    Chunk* getChunk(int x, int z);
    Chunk* getChunkFromBlock(int x, int y, int z);

    void setBlock(Geek::Vector& pos, Block* block, bool updateVisibility = false);
    void setBlock(int x, int y, int z, Block* block, bool updateVisibility = false);
    Block* getBlock(int x, int y, int z);
    Block* getBlock(Geek::Vector& pos);

    void load();
    void save();

    bool update();

    std::vector<SphereMob*> getMobs() { return m_mobs; }
};

#endif //BLOCKY_WORLD_H
