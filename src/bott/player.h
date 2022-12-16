//
// Created by Ian Parker on 14/12/2022.
//

#ifndef BLOCKY_PLAYER_H
#define BLOCKY_PLAYER_H

#include <brick/baseentity.h>

class BottEngine;

class Player : public BaseEntity
{
 private:
    BottEngine* m_engine;

 public:
    explicit Player(BottEngine* engine) : m_engine(engine) {}

    bool canMoveTo(const Geek::Vector &pos) override;
};

#endif //BLOCKY_PLAYER_H
