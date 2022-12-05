//
// Created by Ian Parker on 12/10/2022.
//

#ifndef BLOCKY_MENU_H
#define BLOCKY_MENU_H

#include "overlay.h"

#include <frontier/window.h>

class MenuOverlay : public Overlay
{
 private:
    Frontier::FrontierWindow* m_window;

 public:
    MenuOverlay(Blocky* blocky);

    void init();

    void draw() override;
};


#endif //BLOCKY_MENU_H
