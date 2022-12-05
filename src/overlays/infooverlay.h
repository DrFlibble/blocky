//
// Created by Ian Parker on 31/08/2022.
//

#ifndef BLOCKY_INFOOVERLAY_H
#define BLOCKY_INFOOVERLAY_H

#include "overlay.h"

class InfoOverlay : public Overlay
{
 public:
    explicit InfoOverlay(Blocky* blocky);
    ~InfoOverlay() = default;

    void draw() override;
};


#endif //BLOCKY_INFOOVERLAY_H
