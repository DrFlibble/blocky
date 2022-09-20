//
//

#include "chunk.h"
#include "perlin.h"

using namespace std;
using namespace Geek;
using namespace nlohmann;

Chunk::Chunk(World* world) : Chunk(world, 0, 0)
{
}

Chunk::Chunk(World* world, int chunkX, int chunkZ) : Logger("Chunk"), m_world(world), m_chunkX(chunkX), m_chunkZ(chunkZ)
{
    m_blocks = new Block*[CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT];
    for (int i = 0; i < CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT; i++)
    {
        m_blocks[i] = nullptr;
    }
}

Chunk::~Chunk()
{
    delete m_blocks;
}

void Chunk::init()
{

}
void Chunk::setBlock(int x, int y, int z, Block* block)
{
    if (!validBlockNumber(x, y, z))
    {
        return;
    }
    Block* current = getBlock(x, y, z);
    delete current;

    m_blocks[blockNumber(x, y, z)] = block;
}

Block* Chunk::getBlock(int x, int y, int z)
{
    if (!validBlockNumber(x, y, z))
    {
        return nullptr;
    }
    return m_blocks[blockNumber(x, y, z)];
}

void Chunk::generate(PerlinNoise* perlin)
{
    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        for (int z = 0; z < CHUNK_WIDTH; z++)
        {
#if 1
            int worldX = x + (m_chunkX * 16);
            int worldZ = z + (m_chunkZ * 16);
            float n = perlin->noise((float) worldX * 0.5f, (float) worldZ * 0.5f) + 0.5f;
            auto yo = (int)(n * 16.0f);
            auto y = yo;
            if (y < 0)
            {
                y = 0;
            }

            setBlock(x, y, z, new Block(GRASS));
#if 1
            y--;
            for (; y > (yo / 2); y--)
            {
                setBlock(x, y, z, new Block(DIRT));
            }
            for (; y > 0; y--)
            {
                setBlock(x, y, z, new Block(STONE));
            }
#endif
#else
            setBlock(x, 0, z, new Block(GRASS));
#endif
        }
    }
    updateVisibility();
}

void Chunk::updateVisibility()
{
    m_maxY = 0;
    int visible = 0;
    int hidden = 0;
    for (int x = 0; x < CHUNK_WIDTH - 1; x++)
    {
        for (int y = 0; y < CHUNK_HEIGHT - 1; y++)
        {
            for (int z = 0; z < CHUNK_WIDTH - 1; z++)
            {
                Block* block = getBlock(x, y, z);
                if (block != nullptr)
                {
                    if (getBlock(x - 1, y, z) != nullptr &&
                        getBlock(x + 1, y, z) != nullptr &&
                        getBlock(x, y - 1, z) != nullptr &&
                        getBlock(x, y + 1, z) != nullptr &&
                        getBlock(x, y, z - 1) != nullptr &&
                        getBlock(x, y, z + 1) != nullptr)
                    {
                        block->setVisible(false);
                        hidden++;
                    }
                    else
                    {
                        block->setVisible(true);
                        visible++;
                    }
                    if (y > m_maxY)
                    {
#if 0
                        log(DEBUG, "updateVisibility: m_maxY=%d", y);
#endif
                        m_maxY = y;
                    }
                }
            }
        }
    }
#if 0
    log(DEBUG, "updateVisibility: visible=%d, hidden=%d, m_maxY=%d", visible, hidden, m_maxY);
#endif
}

json Chunk::getJson()
{
    json chunkJson = json::object();
    chunkJson["x"] = m_chunkX;
    chunkJson["y"] = 0;
    chunkJson["z"] = m_chunkZ;
    chunkJson["blocks"] = json::array();

    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        for (int z = 0; z < CHUNK_WIDTH; z++)
        {
            for (int y = 0; y < CHUNK_HEIGHT; y++)
            {
                Block* block = getBlock(x, y, z);
                if (block != nullptr)
                {
                    json blockJson = json::object();
                    blockJson["pos"] = json::object();
                    blockJson["pos"]["x"] = x;
                    blockJson["pos"]["y"] = y;
                    blockJson["pos"]["z"] = z;
                    blockJson["t"] = block->getType();
                    chunkJson["blocks"].push_back(blockJson);
                }
            }
        }
    }

    return chunkJson;
}

void Chunk::fromJson(nlohmann::json json)
{
    m_chunkX = json["x"];
    m_chunkZ = json["z"];

    for (nlohmann::json blockJson : json["blocks"])
    {
        int x = blockJson["pos"]["x"];
        int y = blockJson["pos"]["y"];
        int z = blockJson["pos"]["z"];
        int type = blockJson["t"];
        Block* block = new Block(static_cast<BlockType>(type));
        setBlock(x, y, z, block);
    }
    updateVisibility();
}
