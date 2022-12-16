//
// Created by Ian Parker on 22/09/2022.
//

#ifndef BLOCKY_ENTITY_H
#define BLOCKY_ENTITY_H


#include "blocks.h"
#include <brick/baseentity.h>
#include <geek/core-matrix.h>
#include <geek/core-maths.h>

class World;

class Entity : public BaseEntity
{
 private:
    World* m_world;

 public:
    Entity(World* world) : m_world(world) { }

    bool canMoveTo(const Geek::Vector &pos) override;
};


#endif //BLOCKY_ENTITY_H
