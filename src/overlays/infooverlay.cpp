//
// Created by Ian Parker on 31/08/2022.
//

#include "infooverlay.h"
#include "blocky.h"
#include "world.h"
#include "entity.h"

using namespace Geek;
using namespace Geek::Gfx;

InfoOverlay::InfoOverlay(Blocky* blocky): Overlay(blocky, 300, 30)
{
}

void InfoOverlay::draw()
{
    Surface* infoSurface = getSurface();
    infoSurface->clear(0xff000000);

    World* world = getBlocky()->getWorld();

    wchar_t buf[200];
    swprintf(buf, 200, L"X: %0.2f, Y: %0.2f, Z: %0.2f",
             world->getPlayer().getPosition().x,
             world->getPlayer().getPosition().y,
             world->getPlayer().getPosition().z);
    getBlocky()->getFontManager()->write(getBlocky()->getFont(), infoSurface, 0, 0, buf, 0xffffff, true, nullptr);

    Block* lookingAt = getBlocky()->getLookingAt();
    if (lookingAt != nullptr)
    {
        Vector lookingAtPos = getBlocky()->getLookingAtPos();
        swprintf(buf, 200, L"Looking at: X: %0.2f, Y: %0.2f, Z: %0.2f Type: %d",
                 lookingAtPos.x,
                 lookingAtPos.y,
                 lookingAtPos.z,
                 lookingAt->getType());
        getBlocky()->getFontManager()->write(getBlocky()->getFont(), infoSurface, 0, 12, buf, 0xffffff, true, nullptr);

    }
    Overlay::draw(0, 0);
}
