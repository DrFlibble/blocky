//
// Created by Ian Parker on 25/08/2022.
//

#include "player.h"
#include "entity.h"

using namespace Geek;

static BlockContainer g_nullContainer;

Player::Player(World* world) : Entity(world)
{
    int i;
    for (i = 0; i < 10; i++)
    {
        m_inventory[i].type = EMPTY;
        m_inventory[i].count = 0;
    }
}

BlockContainer& Player::getInventoryByBlockType(BlockType type)
{
    for (BlockContainer& container : m_inventory)
    {
        if (container.type == type)
        {
            return container;
        }
    }
    return g_nullContainer;
}

void Player::addBlockToInventory(BlockType type)
{
    BlockContainer* container;
    bool found = false;
    for (BlockContainer& ci : m_inventory)
    {
        if (ci.type == type)
        {
            container = &ci;
            found = true;
            break;
        }
    }

    if (found)
    {
        container->count++;
    }
    else
    {
        found = false;
        for (BlockContainer& ci : m_inventory)
        {
            if (ci.type == EMPTY)
            {
                container = &ci;
                found = true;
                break;
            }
        }
        if (found)
        {
            container->type = type;
            container->count = 1;
        }
    }
}
