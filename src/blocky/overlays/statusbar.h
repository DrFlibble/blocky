//
// Created by Ian Parker on 31/08/2022.
//

#ifndef BLOCKY_STATUSBAR_H
#define BLOCKY_STATUSBAR_H

#include "brick/overlay.h"
#include "blocky/blocky.h"

class StatusBarOverlay : public Overlay
{
 private:
    Geek::Gfx::Surface* m_heartIcon = nullptr;
    Geek::Gfx::Surface* m_shieldIcon = nullptr;

    Geek::Gfx::Surface* m_dirtIcon = nullptr;
    Geek::Gfx::Surface* m_grassIcon = nullptr;
    Geek::Gfx::Surface* m_stoneIcon = nullptr;

 public:
    explicit StatusBarOverlay(Blocky* blocky);
    ~StatusBarOverlay() = default;

    void draw();
};


#endif //BLOCKY_STATUSBAR_H
