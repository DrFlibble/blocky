//
// Created by Ian Parker on 31/08/2022.
//

#include "statusbar.h"
#include "blocky.h"
#include "world.h"

using namespace Geek;
using namespace Geek::Gfx;

StatusBarOverlay::StatusBarOverlay(Blocky* blocky) : Overlay(blocky, 330, 50)
{
    Surface* heartIcon = Surface::loadPNG("../data/images/37.png");
    m_heartIcon = heartIcon->scaleToFit(16, 16);
    Surface* shieldIcon = Surface::loadPNG("../data/images/183_1.png");
    m_shieldIcon = shieldIcon->scaleToFit(16, 16);

    Surface* dirtSurface = Surface::loadPNG("../dirt-512.png");
    Surface* grassSurface = Surface::loadPNG("../grassblock.png");
    Surface* stoneSurface = Surface::loadPNG("../stone.png");
    m_dirtIcon = dirtSurface->scaleToFit(30, 30);
    m_grassIcon = grassSurface->scaleToFit(30, 30);
    m_stoneIcon = stoneSurface->scaleToFit(30, 30);

    delete dirtSurface;
    delete grassSurface;
    delete stoneSurface;
}

void StatusBarOverlay::draw()
{
    World* world = getBlocky()->getWorld();
    Surface* statusBarSurface = getSurface();
    statusBarSurface->clear(0xff000000);
    int i;
    for (i = 0; i < (int)world->getPlayer().getHealth(); i++)
    {
        statusBarSurface->blit(2 + (16 * i), 2, m_heartIcon, false);
    }

    for (i = 0; i < 3; i++)
    {
        statusBarSurface->blit(
            getWidth() - (16 * (i + 1)),
            2,
            m_shieldIcon,
            false);
    }

    for (i = 0; i < 10; i++)
    {
        int sbx = 2 + (i * 32);
        if (i == world->getPlayer().getInventorySlot())
        {
            statusBarSurface->drawRectFilled(sbx, 18, 32, 32, 0xffffffff);
        }
        else
        {
            //statusBarSurface->drawRect(sbx, 18, 32, 32, 0xffffffff);
        }
        if (i == 0)
        {
            statusBarSurface->blit(sbx + 1, 19, m_stoneIcon);
        }
        else if (i == 1)
        {
            statusBarSurface->blit(sbx + 1, 19, m_grassIcon);
        }
    }

    Overlay::draw(OVERLAY_CENTRE, OVERLAY_END);
}