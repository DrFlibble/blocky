//
//

#ifndef BLOCKY_SHADER_H
#define BLOCKY_SHADER_H

#include <geek/core-logger.h>

#include "brick/engine.h"

class ShaderProgram : private Geek::Logger
{
 private:
    GLuint m_progId = 0;

 public:
    ShaderProgram();
    virtual ~ShaderProgram();

    bool load(GLenum type, const char* source);
    bool link();
    void use() const;
    void unuse() const;

    GLint getUniformLocation(const GLchar *name);
};

class OverlayShader : public ShaderProgram
{
 private:
    GLint m_uniformPositionX = 0;
    GLint m_uniformPositionY = 0;
    GLint m_uniformWidth = 0;
    GLint m_uniformHeight = 0;
    GLint m_uniformTextureWidth = 0;
    GLint m_uniformTextureHeight = 0;

 public:
    OverlayShader();
    ~OverlayShader() override;

    bool load();
    void set(float x, float y, float width, float height, float textureWidth, float textureHeight) const;
};

#endif //BLOCKY_SHADER_H
