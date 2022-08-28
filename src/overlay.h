#ifndef BLOCKY_OVERLAY_H
#define BLOCKY_OVERLAY_H

#include <geek/gfx-surface.h>
#include "texture.h"

class Blocky;

#define OVERLAY_CENTRE -1000
#define OVERLAY_END -2000

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

    int getWidth() const
    {
        return m_width;
    }

    int getHeight() const
    {
        return m_height;
    }
};


#endif //BLOCKY_OVERLAY_H
