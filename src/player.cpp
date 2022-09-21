//
// Created by Ian Parker on 25/08/2022.
//

#include "player.h"

using namespace Geek;

static BlockContainer g_nullContainer;

Player::Player()
{
    m_position.set(0, 16.0, 0);

    int i;
    for (i = 0; i < 10; i++)
    {
        m_inventory[i].type = EMPTY;
    }
}

Player::~Player()
{

}

Matrix4 Player::getMatrix() const
{
    Matrix4 matrix;

    matrix.identity();
    matrix.rotateY(m_heading);   // heading
    matrix.rotateX(m_pitch);   // pitch

    return matrix;
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
