#ifndef BLOCKY_PLAYER_H
#define BLOCKY_PLAYER_H

#include <geek/core-maths.h>
#include <geek/core-matrix.h>

#include "blocks.h"

class Player
{
 private:

    Geek::Vector m_position;

    float m_heading = 0;
    float m_pitch = 0;
    //float m_playerHeight = 1.8;

    // Movement TODO: Vector!
    float m_strafe = 0;
    float m_forward = 0;
    float m_jump = 0;

    float m_health = 10.0;

    BlockContainer m_inventory[10];
    int m_inventorySlot = 0;

 public:
    Player();
    ~Player();

    Geek::Vector& getPosition() { return m_position; }
    void setPosition(Geek::Vector& pos) { m_position = pos; }
    Geek::Matrix4 getMatrix() const;
    float getHeight() { return 1.8; }

    float getHeading() { return m_heading; }

    void setHeading(float heading)
    {
        m_heading = heading;
    }

    float getPitch() { return m_pitch; }

    void setPitch(float pitch)
    {
        m_pitch = pitch;
    }

    float getStrafe() const
    {
        return m_strafe;
    }

    void setStrafe(float mStrafe)
    {
        m_strafe = mStrafe;
    }

    float getForward() const
    {
        return m_forward;
    }

    void setForward(float mForward)
    {
        m_forward = mForward;
    }

    float getJump() const
    {
        return m_jump;
    }

    void setJump(float mJump)
    {
        m_jump = mJump;
    }

    float getHealth()
    {
        return m_health;
    }

    void setHealth(float health)
    {
        m_health = health;
    }

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
