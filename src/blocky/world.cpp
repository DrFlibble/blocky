//
//

#include "world.h"
#include "entity.h"

#include <geek/core-data.h>
#include <nlohmann/json.hpp>

using namespace std;
using namespace Geek;
using namespace nlohmann;


World::World() : Logger("World"), m_player(this)
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
    snprintf(chunkId, 1024, "%d:0:%d", x, z);

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
        Chunk* chunk = new Chunk();
        chunk->fromJson(chunkJson);

        char chunkId[1024];
        snprintf(chunkId, 1024, "%d:0:%d", chunk->getChunkX(), chunk->getChunkZ());
        m_chunks.insert(make_pair(chunkId, chunk));
    }

    m_player.getPosition().x = world["player"]["pos"]["x"];
    m_player.getPosition().y = world["player"]["pos"]["y"];
    m_player.getPosition().z = world["player"]["pos"]["z"];
    m_player.setHeading(world["player"]["heading"]);
    m_player.setPitch(world["player"]["pitch"]);
    m_player.setHealth(world["player"]["health"]);

    for (int i = 0; i < 10; i++)
    {
        json invSlot = world["player"]["inventory"][i];
        if (!invSlot.empty())
        {
            m_player.getInventorySlot(i).type = invSlot["type"];
            m_player.getInventorySlot(i).count = invSlot["count"];
        }
    }
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

    for (int i = 0; i < 10; i++)
    {
        BlockContainer container = m_player.getInventorySlot(i);
        player["inventory"][i]["type"] = container.type;
        player["inventory"][i]["count"] = container.count;
    }

    world["player"] = player;

    for (auto& it : this->m_chunks)
    {
        auto chunk = it.second;
        json chunkJson = chunk->getJson();
        world["chunks"].push_back(chunkJson);
    }

    auto bson = world.to_cbor(world);
    Data* data = new Data((char*)bson.data(), bson.size());
    data->writeCompressed("test.world", DataCompression::GZIP);
}

float mobpos(float player, float r)
{
    if (r < 0)
    {
        r -= 5;
    }
    else
    {
        r += 5;
    }
    return player + r;
}

bool World::update()
{
    bool changed = m_player.update();

    int r = m_random->range(0, 100);
    if (r == 1)
    {
        SphereMob* newMob = new SphereMob(this);
        Vector v(
            mobpos(m_player.getPosition().x, m_random->ranged(-10, 10)),
            mobpos(m_player.getPosition().y, m_random->ranged(-10, 10)),
            mobpos(m_player.getPosition().z, m_random->ranged(-10, 10)));
        newMob->setPosition(v);
        m_mobs.push_back(newMob);
    }

    for (SphereMob* mob : m_mobs)
    {
        r = m_random->range(0, 10);
        if (r == 1)
        {
            mob->setHeading(mob->getHeading() + 45);
        }
        else if (r == 2)
        {
            mob->setHeading(mob->getHeading() - 45);
        }
        else if (r == 3)
        {
            mob->setForward(0.1f);
        }
        mob->update();
    }

    return changed;
}
