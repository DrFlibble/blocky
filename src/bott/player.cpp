//
// Created by Ian Parker on 14/12/2022.
//

#include "player.h"
#include "bott.h"
#include "terrain.h"

bool Player::canMoveTo(const Geek::Vector &pos)
{
    float height = m_engine->getTerrain()->heightAt(pos.x, pos.z);
    return pos.y >= height;
}
