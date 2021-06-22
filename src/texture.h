#ifndef BLOCKY_TEXTURE_H
#define BLOCKY_TEXTURE_H

#include <geek/gfx-surface.h>
#include <geek/core-logger.h>

class Texture : private Geek::Logger
{
 private:
    unsigned int m_texture = 0;
    unsigned int m_sampler = 0;
    bool m_textureValid = false;

 protected:
    Geek::Gfx::Surface* m_surface = nullptr;

    bool generateTexture();

 public:
    explicit Texture(Geek::Gfx::Surface* surface);
    virtual ~Texture();

    void bind();
};

#endif //BLOCKY_TEXTURE_H
