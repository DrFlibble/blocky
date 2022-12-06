//
// Created by Ian Parker on 28/08/2022.
//

#include "crosshair.h"

using namespace Geek::Gfx;

CrossHairOverlay::CrossHairOverlay(Blocky* blocky) : Overlay(blocky, 16, 16)
{
    Surface* crosshair = getSurface();
    crosshair->clear(0xff000000);
    crosshair->drawRect(0, 7, 6, 2, 0xffffffff);
    crosshair->drawRect(9, 7, 6, 2, 0xffffffff);
    crosshair->drawRect(7, 0, 2, 6, 0xffffffff);
    crosshair->drawRect(7, 9, 2, 6, 0xffffffff);
}

void CrossHairOverlay::draw()
{
    Overlay::draw(OVERLAY_CENTRE, OVERLAY_CENTRE);
}
