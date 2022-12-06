//
// Created by Ian Parker on 28/08/2022.
//

#ifndef BLOCKY_CROSSHAIR_H
#define BLOCKY_CROSSHAIR_H

#include "libbrick/overlay.h"

class CrossHairOverlay : public Overlay
{
 private:
 public:
    explicit CrossHairOverlay(Blocky* blocky);
    ~CrossHairOverlay() = default;

    void draw();
};


#endif //BLOCKY_CROSSHAIR_H
