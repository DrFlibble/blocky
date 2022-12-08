//
// Created by Ian Parker on 24/08/2022.
//

#include "brick/overlay.h"
#include "brick/engine.h"
#include "brick/shader.h"
#include "utils.h"

Overlay::Overlay(BrickEngine* engine) : m_engine(engine), m_width(0), m_height(0)
{
}

Overlay::Overlay(BrickEngine* engine, int width, int height) : m_engine(engine), m_width(width), m_height(height)
{
    resize(m_width, m_height);
}

Overlay::~Overlay() = default;

void Overlay::resize(int width, int height)
{
    if (m_overlaySurface != nullptr && m_overlayTexture != nullptr && m_width == width && m_height == height)
    {
        return;
    }

    delete m_overlaySurface;
    delete m_overlayTexture;

    m_width = width;
    m_height = height;
    int widthPow = powerOfTwo(width);
    int heightPow = powerOfTwo(height);

    int size = MAX(widthPow, heightPow);

    m_overlaySurface = new Geek::Gfx::Surface(size, size, 4);
    m_overlaySurface->clear(0x00ff00);
    m_overlayTexture = new Texture(m_overlaySurface);
}

void Overlay::draw(int x, int y)
{
    if (m_overlayTexture == nullptr || m_overlaySurface == nullptr)
    {
        return;
    }

    OverlayShader* shader = m_engine->getOverlayShader();
    shader->use();
    m_overlayTexture->generateTexture();
    m_overlayTexture->bind();

    if (x == OVERLAY_CENTRE)
    {
        x = (m_engine->getWidth() / 2) - (m_width / 2);
    }
    else if (x == OVERLAY_END)
    {
        x = m_engine->getWidth() - m_width;
    }

    if (y == OVERLAY_CENTRE)
    {
        y = (m_engine->getHeight() / 2) - (m_height / 2);
    }
    else if (y == OVERLAY_END)
    {
        y = m_engine->getHeight() - m_height;
    }

    float sw = (float)m_engine->getWidth();
    float sh = (float)m_engine->getHeight();
    shader->set(
        (float)x / sw,
        (float)y / sh,
        (float)m_width / sw,
        (float)m_height / sh,
        (float)m_width / (float)m_overlaySurface->getWidth(),
        (float)m_height / (float)m_overlaySurface->getHeight());

    glBindVertexArray(m_engine->getOverlayVAO());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
