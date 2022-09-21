
#include "texture.h"
#include "blocky.h"
#include "utils.h"

using namespace Geek;
using namespace Geek::Gfx;

Texture::Texture(const char* filename) : Logger("Texture")
{
    init();
    m_surface = Geek::Gfx::Surface::loadPNG(filename);
}

Texture::Texture(Surface* surface) : Logger("Texture"), m_surface(surface)
{
    init();
}

void Texture::init()
{
    GL(glGenTextures(1, &m_texture));
    GL(glGenSamplers(1, &m_sampler));
}

Texture::~Texture()
{
    if (m_textureValid)
    {
        glDeleteTextures(1, &m_texture );
    }
}

void Texture::bind()
{
    if (!m_textureValid)
    {
        generateTexture();
    }
    GL(glBindTexture(GL_TEXTURE_2D, m_texture));
    GL(glBindSampler(m_texture, m_sampler));
}

bool Texture::generateTexture()
{
    int texWidth = powerOfTwo((int)m_surface->getWidth());
    int texHeight = powerOfTwo((int)m_surface->getHeight());
#if 0
    log(DEBUG, "generateTexture: surface: %d, %d, texture: %d, %d", m_surface->getWidth(), m_surface->getHeight(), texWidth, texHeight);
#endif

    if (texWidth != m_surface->getWidth() ||
        texHeight != m_surface->getHeight())
    {
        log(ERROR, "generateTexture: Handle non-power-of-two surface size");
        return false;
    }

    GL(glBindTexture(GL_TEXTURE_2D, m_texture));
    GL(glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        texWidth,
        texHeight,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        m_surface->getData()));

    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GL(glGenerateMipmap(GL_TEXTURE_2D));
    m_textureValid = true;
    return true;
}

