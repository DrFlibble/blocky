#ifndef BRICK_OVERLAY_H
#define BRICK_OVERLAY_H

#include <geek/gfx-surface.h>
#include "texture.h"

class BrickEngine;

#define OVERLAY_CENTRE (-1000)
#define OVERLAY_END (-2000)

class Overlay
{
 protected:
    BrickEngine* m_engine;

    int m_width = 0;
    int m_height = 0;
    Geek::Gfx::Surface* m_overlaySurface = nullptr;
    Texture* m_overlayTexture = nullptr;

    bool m_visible = true;

 public:
    explicit Overlay(BrickEngine* engine);
    Overlay(BrickEngine* engine, int width, int height);
    virtual ~Overlay();

    void resize(int width, int height);
    Geek::Gfx::Surface* getSurface() { return m_overlaySurface; }

    virtual void draw() {draw(0, 0); }
    void draw(int x, int y);

    BrickEngine* getEngine() const
    {
        return m_engine;
    }

    int getWidth() const
    {
        return m_width;
    }

    int getHeight() const
    {
        return m_height;
    }

    bool isVisible() const
    {
        return m_visible;
    }

    void setVisible(bool mVisible)
    {
        m_visible = mVisible;
    }
};


#endif //BRICK_OVERLAY_H
