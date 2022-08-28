//
// Created by Ian Parker on 25/08/2022.
//

#include "player.h"

using namespace Geek;

Player::Player()
{
    m_position.set(0, 16.0, 0);

    int i;
    for (i = 0; i < 10; i++)
    {
        m_inventory[i].type = EMPTY;
    }
}

Player::~Player()
{

}

Matrix4 Player::getMatrix() const
{
    Matrix4 matrix;

    matrix.identity();
    matrix.rotateY(m_heading);   // heading
    matrix.rotateX(m_pitch);   // pitch

    return matrix;
}
