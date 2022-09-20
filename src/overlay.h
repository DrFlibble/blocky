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
    virtual ~Overlay();

    Geek::Gfx::Surface* getSurface() { return m_overlaySurface; }

    virtual void draw() {draw(0, 0); }
    void draw(int x, int y);

    Blocky* getBlocky() const
    {
        return m_blocky;
    }

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
