//
// Created by Ian Parker on 22/09/2022.
//


#include "entity.h"
#include "chunk.h"

#include <geek/core-maths.h>
#include <SDL_timer.h>

using namespace Geek;

bool Entity::update()
{
    bool changed = false;
    double headingSin = sin(m_heading * (M_PI / 180.0));
    double headingCos = cos(m_heading * (M_PI / 180.0));

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
                float c = cos((1.0f-m_jump) * 1.0f * M_PI);
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
        int entityX = floor(checkPos.x);
        int entityY = floor(checkPos.y);
        int entityZ = floor(checkPos.z);
        Block* nextBlockUnderEntity = m_world->getBlock(
            entityX,
            ceil(checkPos.y) - 1,
            entityZ);
        bool hasBlockUnderEntity = nextBlockUnderEntity != nullptr;
        if (moving)
        {
            Block* nextBlockAtEntity = m_world->getBlock(entityX, entityY, entityZ);
            Block* nextBlockAboveEntity = m_world->getBlock(
                entityX,
                entityY + 1,
                entityZ);

            bool hasBlockAtEntity = nextBlockAtEntity != nullptr;
            bool hasBlockAboveEntity = nextBlockAboveEntity != nullptr;

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
                m_position.y = entityY + 1;
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
            if (m_position.y < entityY)
            {
                m_position.y = entityY;
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

Geek::Matrix4 Entity::getMatrix() const
{
    Geek::Matrix4 matrix;

    matrix.identity();
    matrix.rotateY(m_heading);   // heading
    matrix.rotateX(m_pitch);   // pitch

    return matrix;
}
