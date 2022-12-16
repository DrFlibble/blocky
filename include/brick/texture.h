#ifndef BRICK_TEXTURE_H
#define BRICK_TEXTURE_H

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

    void init();

 public:
    explicit Texture(const char* filename);
    explicit Texture(Geek::Gfx::Surface* surface);
    virtual ~Texture();

    void bind();
    bool generateTexture();

    [[nodiscard]] Geek::Gfx::Surface* getSurface() const { return m_surface; }
};

#endif //BRICK_TEXTURE_H
