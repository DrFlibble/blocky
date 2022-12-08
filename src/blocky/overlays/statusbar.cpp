//
// Created by Ian Parker on 31/08/2022.
//

#include "statusbar.h"
#include "blocky/blocky.h"
#include "blocky/world.h"
#include "blocky/entity.h"

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
    World* world = ((Blocky*) getEngine())->getWorld();
    Surface* statusBarSurface = getSurface();
    statusBarSurface->clear(0x00000000);
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
            statusBarSurface->drawRectFilled(sbx, 18, 32, 32, 0xff0000ff);
        }
        else
        {
            statusBarSurface->drawRectFilled(sbx, 18, 32, 32, 0xffffffff);
        }

        BlockContainer container = world->getPlayer().getInventorySlot(i);
        Surface* icon = nullptr;
        switch (container.type)
        {
            case GRASS:
                icon = m_grassIcon;
                break;
            case DIRT:
                icon = m_dirtIcon;
                break;
            case STONE:
                icon = m_stoneIcon;
                break;
            default:
                // Empty
                break;
        }
        if (icon != nullptr && container.count > 0)
        {
            statusBarSurface->blit(sbx + 1, 19, icon);

            int width = 0;
            wchar_t buf[200];
            swprintf(buf, 200, L"%d", container.count);

            getEngine()->getFontManager()->write(getEngine()->getFont(), nullptr, sbx + 2, 20, buf, 0xffffff, false, &width);
            statusBarSurface->drawRectFilled(sbx + 2, 20, width, 14, 0x0);
            getEngine()->getFontManager()->write(getEngine()->getFont(), statusBarSurface, sbx + 2, 20, buf, 0xffffff, true, nullptr);
        }
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Overlay::draw(OVERLAY_CENTRE, OVERLAY_END);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}
