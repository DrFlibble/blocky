#ifndef BLOCKY_PLAYER_H
#define BLOCKY_PLAYER_H

#include <geek/core-maths.h>
#include <geek/core-matrix.h>

#include "entity.h"
#include "blocks.h"

class Player : public Entity
{
 private:

    //float m_playerHeight = 1.8;

    BlockContainer m_inventory[10];
    int m_inventorySlot = 0;

 public:
    Player(World* world);
    ~Player() = default;

    float getHeight() { return 1.8; }

    BlockContainer& getInventorySlot(int slot)
    {
        return m_inventory[slot];
    }
    BlockContainer& getInventoryByBlockType(BlockType type);
    void addBlockToInventory(BlockType type);

    int getInventorySlot() const
    {
        return m_inventorySlot;
    }

    void setInventorySlot(int mInventorySlot)
    {
        m_inventorySlot = mInventorySlot;
    }
};

#endif //BLOCKY_PLAYER_H
