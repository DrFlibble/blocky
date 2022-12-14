#ifndef BLOCKY_CHUNK_H
#define BLOCKY_CHUNK_H

#include <geek/core-logger.h>
#include "world.h"
#include "blocks.h"

#include <nlohmann/json.hpp>

class PerlinNoise;
class World;

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 256

class Block
{
 private:
    BlockType m_type;
    bool m_visible;

 public:
    explicit Block(BlockType type) : m_type(type), m_visible(true) {}
    ~Block() = default;

    [[nodiscard]] BlockType getType() const
    {
        return m_type;
    }

    [[nodiscard]] bool isVisible() const
    {
        return m_visible && m_type != AIR;
    }

    void setVisible(bool visible)
    {
        m_visible = visible;
    }
};

class Chunk : private Geek::Logger
{
 private:
    int m_chunkX;
    int m_chunkZ;
    int m_maxY = 0;
    Block** m_blocks;

    [[nodiscard]] static bool validBlockNumber(int x, int y, int z)
    {
        return !(x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_WIDTH);
    }

    [[nodiscard]] static int blockNumber(int x, int y, int z)
    {
        return (x * CHUNK_WIDTH * CHUNK_HEIGHT) + (z * CHUNK_HEIGHT) + y;
    }

 public:
    Chunk();
    Chunk(int chunkX, int chunkZ);
    ~Chunk();

    void setBlock(int x, int y, int z, Block* block);
    Block* getBlock(int x, int y, int z);

    void generate(PerlinNoise* perlin);
    void updateVisibility();

    [[nodiscard]] int getChunkX() const
    {
        return m_chunkX;
    }

    [[nodiscard]] int getChunkZ() const
    {
        return m_chunkZ;
    }

    [[nodiscard]] int getMaxY() const
    {
        return m_maxY;
    }

    void fromJson(nlohmann::json json);
    nlohmann::json getJson();
};

#endif //BLOCKY_CHUNK_H
