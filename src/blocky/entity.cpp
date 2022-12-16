//
// Created by Ian Parker on 22/09/2022.
//


#include "entity.h"
#include "chunk.h"

#include <geek/core-maths.h>
#include <SDL_timer.h>

using namespace Geek;

bool Entity::canMoveTo(const Geek::Vector &pos)
{
    Block* block = m_world->getBlock(
        floor(pos.x),
        floor(pos.y),
        floor(pos.z));
    return block == nullptr;
}
