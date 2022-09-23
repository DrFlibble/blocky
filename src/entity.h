//
// Created by Ian Parker on 22/09/2022.
//

#ifndef BLOCKY_ENTITY_H
#define BLOCKY_ENTITY_H


#include "blocks.h"
#include <geek/core-matrix.h>
#include <geek/core-maths.h>

class World;

class Entity
{
 private:
    World* m_world;

    // Movement TODO: Vector!
    float m_strafe = 0;
    float m_forward = 0;
    float m_jump = 0;
    float m_health = 10.0;

    uint64_t m_lastMillis;

 protected:
    float m_heading = 0;
    float m_pitch = 0;
    Geek::Vector m_position;

 public:
    Entity(World* world) : m_world(world)
    {
        m_position.set(0, 16.0, 0);
    }

    bool update();

    Geek::Vector& getPosition() { return m_position; }

    void setPosition(Geek::Vector& pos) { m_position = pos; }

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

    Geek::Matrix4 getMatrix() const;

};


#endif //BLOCKY_ENTITY_H
