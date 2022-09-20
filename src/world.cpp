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
    m_random = new Geek::Core::Random();
    m_seed = (uint64_t)m_random->rand32() << 32;
    m_seed |= (uint64_t)m_random->rand32();
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

void World::load()
{
    Data* data = new Data();
    bool res;
    res = data->loadCompressed("test.world", DataCompression::AUTO);
    if (!res)
    {
        return;
    }
    log(DEBUG, "load: data=%p, length=%d", data->getData(), data->getLength());
    json world = json::from_cbor(data->getData(), data->getData() + data->getLength());

    log(DEBUG, "load: player=%s", to_string(world["player"]).c_str());
    log(DEBUG, "load: chunks=%d", world["chunks"].size());

    if (world.contains("seed"))
    {
        m_seed = world["seed"];
    }

    for (json chunkJson : world["chunks"])
    {
        log(DEBUG, "load: Chunk: x=%d, z=%d", (int)chunkJson["x"], (int)chunkJson["z"]);
        Chunk* chunk = new Chunk(this);
        chunk->fromJson(chunkJson);

        char chunkId[1024];
        sprintf(chunkId, "%d:0:%d", chunk->getChunkX(), chunk->getChunkZ());
        m_chunks.insert(make_pair(chunkId, chunk));
    }

    m_player.getPosition().x = world["player"]["pos"]["x"];
    m_player.getPosition().y = world["player"]["pos"]["y"];
    m_player.getPosition().z = world["player"]["pos"]["z"];
    m_player.setHeading(world["player"]["heading"]);
    m_player.setPitch(world["player"]["pitch"]);
    m_player.setHealth(world["player"]["health"]);
}

void World::save()
{
    json world = json::object();

    world["seed"] = m_seed;
    world["chunks"] = json::array();

    json player;
    player["pos"]["x"] = m_player.getPosition().x;
    player["pos"]["y"] = m_player.getPosition().y;
    player["pos"]["z"] = m_player.getPosition().z;
    player["heading"] = m_player.getHeading();
    player["pitch"] = m_player.getPitch();
    player["health"] = m_player.getHealth();
    world["player"] = player;

    for (auto it : this->m_chunks)
    {
        auto chunk = it.second;
        json chunkJson = chunk->getJson();
        world["chunks"].push_back(chunkJson);
    }

    auto bson = world.to_cbor(world);
    Data* data = new Data((char*)bson.data(), bson.size());
    data->writeCompressed("test.world", DataCompression::GZIP);
}
