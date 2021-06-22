//
//

#include <stdlib.h>
#include "chunk.h"

using namespace std;
using namespace Geek;

Chunk::Chunk(int chunkX, int chunkZ) : Logger("Chunk")
{
    m_chunkX = chunkX;
    m_chunkZ = chunkZ;

    m_width = 16;
    m_height = 256;
    m_blocks = new Block*[m_width * m_width * m_height];
    for (int i = 0; i < m_width * m_width * m_height; i++)
    {
        m_blocks[i] = nullptr;
    }
}

Chunk::~Chunk()
{
}

void Chunk::setBlock(int x, int y, int z, Block* block)
{
    /*
    Block* current = getBlock(x, y, z);
    if (current != nullptr)
    {
        // TODO DELETE!
    }
     */

    m_blocks[blockNumber(x, y, z)] = block;
}

Block* Chunk::getBlock(int x, int y, int z)
{
    return m_blocks[blockNumber(x, y, z)];
}

int Chunk::blockNumber(int x, int y, int z)
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height || z < 0 || z >= m_height)
    {
        log(ERROR, "blockNumber: Invalid position: x=%d, y=%d, z=%d", x, y, z);
        abort();
    }
    return (x * m_width * m_height) + (z * m_height) + y;
}

