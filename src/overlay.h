#ifndef BLOCKY_OVERLAY_H
#define BLOCKY_OVERLAY_H

#include <geek/gfx-surface.h>
#include "texture.h"

class Blocky;

class Overlay
{
 private:
    Blocky* m_blocky;

    int m_width;
    int m_height;
    Geek::Gfx::Surface* m_overlaySurface = nullptr;
    Texture* m_overlayTexture = nullptr;

 public:
    Overlay(Blocky* blocky, int width, int height);
    ~Overlay();

    Geek::Gfx::Surface* getSurface() { return m_overlaySurface; }

    void draw(int x, int y);

};


#endif //BLOCKY_OVERLAY_H
