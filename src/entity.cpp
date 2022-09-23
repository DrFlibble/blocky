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

            checkPos.x +=
                headingSin * (m_forward * 2) +
                headingCos * (m_strafe * 2);
            checkPos.z +=
                headingCos * -(m_forward * 2) +
                headingSin * (m_strafe * 2);
            m_forward *= 0.75f;
            m_strafe *= 0.75f;

            if (m_jump > 0)
            {
                if (m_jump > 0.5)
                {
                    m_position.y += 0.2;
                }
                else
                {
                    m_position.y -= 0.2;
                }
                m_jump -= 0.1;
            }
        }
        m_lastMillis = time;

        bool canMove = true;
        int playerX = floor(checkPos.x);
        int playerY = floor(checkPos.y);
        int playerZ = floor(checkPos.z);
        Block* nextBlockUnderPlayer = m_world->getBlock(
            playerX,
            playerY - 1,
            playerZ);
        bool hasBlockUnderPlayer = nextBlockUnderPlayer != nullptr;
        if (moving)
        {
            Block* nextBlockAtPlayer = m_world->getBlock(playerX, playerY, playerZ);
            Block* nextBlockAbovePlayer = m_world->getBlock(
                playerX,
                playerY + 1,
                playerZ);

            bool hasBlockAtPlayer = nextBlockAtPlayer != nullptr;
            bool hasBlockAbovePlayer = nextBlockAbovePlayer != nullptr;

            if (hasBlockAbovePlayer)
            {
                canMove = false;
            }
            else if (hasBlockUnderPlayer && !hasBlockAtPlayer)
            {
                canMove = true;
            }
            else if (hasBlockAtPlayer)
            {
                canMove = true;
                m_position.y++;
                hasBlockUnderPlayer = true;
            }
        }

        if (canMove && !hasBlockUnderPlayer && m_jump <= 0)
        {
            // Falling!
            if (m_position.y > 0)
            {
                m_position.y -= 1;
            }
        }

        if (!canMove)
        {
            // Nope! No moving for you!
            m_position = origPosition;
            m_forward = 0.0;
            m_strafe = 0.0;
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
