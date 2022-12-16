//
// Created by Ian Parker on 22/09/2022.
//

#ifndef BRICK_BASE_ENTITY_H_
#define BRICK_BASE_ENTITY_H_

#include <geek/core-matrix.h>
#include <geek/core-maths.h>
#include <geek/core-logger.h>

union SDL_Event;

class BaseEntity : public Geek::Logger
{
 private:
    // Movement TODO: Vector!
    float m_strafe = 0;
    float m_forward = 0;
    float m_jump = 0;
    float m_health = 10.0;

    uint64_t m_lastMillis = 0;

    float m_controllerForward = 0.0;
    float m_controllerStrafe = 0.0;
    float m_controllerHeading = 0.0;
    float m_controllerPitch = 0.0;

 protected:
    float m_heading = 0;
    float m_pitch = 0;
    Geek::Vector m_position;

 public:
    BaseEntity() : Geek::Logger("BaseEntity")
    {
        m_position.set(0, 16.0, 0);
    }

    virtual bool canMoveTo(const Geek::Vector& pos) = 0;

    bool update();

    bool handleEvent(SDL_Event* event);

    Geek::Vector& getPosition() { return m_position; }

    void setPosition(Geek::Vector& pos) { m_position = pos; }

    float getHeading() const { return m_heading; }

    void setHeading(float heading)
    {
        m_heading = heading;
    }

    float getPitch() const { return m_pitch; }

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

#endif
