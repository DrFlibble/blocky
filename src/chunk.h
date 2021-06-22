#ifndef BLOCKY_CHUNK_H
#define BLOCKY_CHUNK_H

#include <geek/core-logger.h>

enum BlockType
{
    DIRT,
    GRASS
};

class Block
{
 private:
    BlockType m_type;

 public:
    Block(BlockType type) { m_type = type; }
    ~Block() {}

    BlockType getType() const
    {
        return m_type;
    }
};

class Chunk : private Geek::Logger
{
 private:
    int m_chunkX;
    int m_chunkZ;
    Block** m_blocks;
    int m_width;

 private:
    int m_height;

    [[nodiscard]] int blockNumber(int x, int y, int z);

 public:
    Chunk(int chunkX, int chunkZ);
    ~Chunk();

    void setBlock(int x, int y, int z, Block* block);
    Block* getBlock(int x, int y, int z);

    int getChunkX() const
    {
        return m_chunkX;
    }

    int getChunkZ() const
    {
        return m_chunkZ;
    }

    [[nodiscard]] int getWidth() const
    {
        return m_width;
    }

    [[nodiscard]] int getHeight() const
    {
        return m_height;
    }
};

#endif //BLOCKY_CHUNK_H
