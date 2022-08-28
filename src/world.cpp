//
//

#include "world.h"

#include <geek/core-data.h>
#include <nlohmann/json.hpp>

using namespace std;
using namespace Geek;
using namespace nlohmann;


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

void World::save()
{
    json world = json::object();

    world["chunks"] = json::array();

    json player;
    player["pos"]["x"] = m_player.getPosition().x;
    player["pos"]["y"] = m_player.getPosition().y;
    player["pos"]["z"] = m_player.getPosition().z;
    world["player"] = player;

    for (auto it : this->m_chunks)
    {
        auto chunk = it.second;
        log(DEBUG, "save: Saving chunk: %p", chunk);
        json chunkJson = chunk->getJson();
        world["chunks"].push_back(chunkJson);
    }
    string str = to_string(world);
    log(DEBUG, "save: str:\n%s\n", str.c_str());

    auto bson = world.to_cbor(world);
    Data* data = new Data((char*)bson.data(), bson.size());
    data->writeCompressed("test.world", DataCompression::GZIP);

}
