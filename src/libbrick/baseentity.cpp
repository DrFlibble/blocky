//
// Created by Ian Parker on 22/09/2022.
//


#include <brick/baseentity.h>

#include <geek/core-maths.h>
#include <SDL_events.h>
#include <SDL_timer.h>
#include <geek/core-logger.h>

using namespace Geek;

bool BaseEntity::update()
{
    bool changed = false;
    double headingSin = sin(m_heading * (M_PI / 180.0));
    double headingCos = cos(m_heading * (M_PI / 180.0));

    if (fabs(m_controllerForward) > EPSILON)
    {
        setForward(0.2 * m_controllerForward);
    }
    if (fabs(m_controllerStrafe) > EPSILON)
    {
        setStrafe(0.2 * m_controllerStrafe);
    }
    if (fabs(m_controllerHeading) > EPSILON)
    {
        setHeading(getHeading() + (4.0 * m_controllerHeading));
    }
    if (fabs(m_controllerPitch) > EPSILON)
    {
        setPitch(getPitch() + (3.0 * m_controllerPitch));
    }

    uint64_t time = SDL_GetTicks();
    if ((time - m_lastMillis) > (1000 / 60))
    {
        Vector origPosition = m_position;
        bool moving =
            fabs(m_forward) > EPSILON ||
            fabs(m_strafe) > EPSILON ||
            m_jump > 0;

        Vector checkPos = m_position;
        if (moving)
        {
            m_position.x += headingSin * m_forward + headingCos * m_strafe;
            m_position.z += headingCos * -m_forward + headingSin * m_strafe;

            m_forward *= 0.75f;
            m_strafe *= 0.75f;

            checkPos.x +=
                headingSin * (m_forward * 2) +
                headingCos * (m_strafe * 2);
            checkPos.z +=
                headingCos * -(m_forward * 2) +
                headingSin * (m_strafe * 2);

            if (m_jump > 0)
            {
                float c = cosf((1.0f-m_jump) * 1.0f * M_PI);
                printf("update: JUMP: %0.2f -> %0.2f\n", m_jump, c);
                //if (m_jump > 0.5)
                {
                    m_position.y += c * 0.3;
                }
                m_jump -= 1.0f / 11.0f;
                //else
                //{
                    //m_position.y -= c * 0.3;
                //}
            }
        }
        m_lastMillis = time;

        bool canMove = true;
        /*
        int entityX = floor(checkPos.x);
        int entityY = floor(checkPos.y);
        int entityZ = floor(checkPos.z);
         */
        /*
        Block* nextBlockUnderEntity = m_world->getBlock(
            entityX,
            ceilf(checkPos.y) - 1,
            entityZ);
        bool hasBlockUnderEntity = nextBlockUnderEntity != nullptr;
            */
        bool hasBlockUnderEntity = !canMoveTo(checkPos + Vector(0, -1, 0));
        if (moving)
        {
            /*
            Block* nextBlockAtEntity = m_world->getBlock(entityX, entityY, entityZ);
            Block* nextBlockAboveEntity = m_world->getBlock(
                entityX,
                entityY + 1,
                entityZ);
            */

            bool hasBlockAtEntity = !canMoveTo(checkPos);
            bool hasBlockAboveEntity = !canMoveTo(checkPos + Vector(0, 2, 0));

            if (hasBlockAboveEntity)
            {
                canMove = false;
            }
            else if (hasBlockUnderEntity && !hasBlockAtEntity)
            {
                canMove = true;
            }
            else if (hasBlockAtEntity)
            {
                canMove = true;
                m_position.y = checkPos.y + 1;
                hasBlockUnderEntity = true;
            }
        }

        if (/*canMove &&*/ !hasBlockUnderEntity && m_jump <= EPSILON)
        {
            // Falling!
            if (m_position.y > 0)
            {
                m_position.y -= 0.2;
            }
        }
        else if (hasBlockUnderEntity)
        {
            if (m_position.y < checkPos.y)
            {
                m_position.y = checkPos.y;
            }
        }

        if (!canMove)
        {
            // Nope! No moving for you!
            m_position = origPosition;
            m_forward = 0.0;
            m_strafe = 0.0;
        }

        if (m_position.y <= 1)
        {
            m_position.y = 1;
        }

        changed = true;
    }
    return changed;
}

Geek::Matrix4 BaseEntity::getMatrix() const
{
    Geek::Matrix4 matrix;

    matrix.identity();
    matrix.rotateY(m_heading);   // heading
    matrix.rotateX(m_pitch);   // pitch

    return matrix;
}

bool BaseEntity::handleEvent(SDL_Event* event)
{
    bool handled = false;
    if (event->type == SDL_KEYDOWN)
    {
        handled = true;
        if (event->key.keysym.sym == SDLK_w)
        {
            if (event->key.keysym.mod & KMOD_SHIFT)
            {
                setForward(0.4f);
            }
            else
            {
                setForward(0.2f);
            }
        }
        else if (event->key.keysym.sym == SDLK_s)
        {
            setForward(-0.2f);
        }
        else if (event->key.keysym.sym == SDLK_a)
        {
            setStrafe(-0.2f);
        }
        else if (event->key.keysym.sym == SDLK_d)
        {
            setStrafe(0.2f);
        }
        else if (getJump() <= 0 && event->key.keysym.sym == SDLK_SPACE)
        {
            setJump(1.0f);
        }
        else
        {
            handled = false;
        }
    }
    else if (event->type == SDL_MOUSEMOTION)
    {
        float heading = getHeading();
        float pitch = getPitch();
        heading += (float) (event->motion.xrel) * 0.1f;
        pitch += (float) (event->motion.yrel) * 0.1f;
        if (pitch > 90.0)
        {
            pitch = 90.0;
        }
        else if (pitch < -90.0)
        {
            pitch = -90.0;
        }
        if (heading > 360)
        {
            heading -= 360;
        }
        else if (heading < 0)
        {
            heading += 360;
        }

        setHeading(heading);
        setPitch(pitch);
        handled = true;
    }
    else if (event->type == SDL_CONTROLLERAXISMOTION)
    {
        float pct = (float)event->caxis.value / (float)0x7fff;
        if (fabs(pct) < 0.5)
        {
            pct = 0;
        }

        log(DEBUG, "handleEvent: Controller Axis: axis=%d, value=%d, pct=%0.2f%%",
            event->caxis.axis,
            event->caxis.value,
            pct * 100.0f);

        if (event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
        {
            m_controllerForward = -pct;
        }
        if (event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
        {
            m_controllerStrafe = pct;
        }
        else if (event->caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX)
        {
            m_controllerHeading = pct;
        }
        else if (event->caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY)
        {
            m_controllerPitch = pct;
        }
    }

    return handled;
}
