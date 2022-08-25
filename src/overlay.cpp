//
// Created by Ian Parker on 24/08/2022.
//

#include "overlay.h"
#include "blocky.h"
#include "shader.h"
#include "utils.h"

#include <OpenGL/gl3.h>

Overlay::Overlay(Blocky* blocky, int width, int height)
{
    m_blocky = blocky;
    m_width = width;
    m_height = height;
    int widthPow = powerOfTwo(width);
    int heightPow = powerOfTwo(height);

    int size = MAX(widthPow, heightPow);

    m_overlaySurface = new Geek::Gfx::Surface(size, size, 4);
    m_overlaySurface->clear(0x00ff00);
    m_overlayTexture = new Texture(m_overlaySurface);
}

Overlay::~Overlay() = default;

void Overlay::draw(int x, int y)
{

    OverlayShader* shader = m_blocky->getOverlayShader();
    shader->use();
    m_overlayTexture->generateTexture();
    m_overlayTexture->bind();
    shader->set(
        (float)x / (float)m_blocky->getWidth(),
        (float)y / (float)m_blocky->getHeight(),
        (float)m_width / (float)m_blocky->getWidth(),
        (float)m_height / (float)m_blocky->getHeight(),
        (float)m_width / (float)m_overlaySurface->getWidth(),
        (float)m_height / (float)m_overlaySurface->getHeight());

    glBindVertexArray(m_blocky->getOverlayVAO());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
