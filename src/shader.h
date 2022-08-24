//
//

#ifndef BLOCKY_SHADER_H
#define BLOCKY_SHADER_H

#include "blocky.h"

#include <geek/core-logger.h>

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

class MainShader : public ShaderProgram
{
 private:
    GLint m_uniformMatrixModelView = 0;
    GLint m_uniformMatrixModelViewProjection = 0;
    GLint m_uniformMatrixNormal = 0;
    GLint m_uniformLightPosition = 0;
    GLint m_uniformLightAmbient = 0;
    GLint m_uniformLightDiffuse = 0;
    GLint m_uniformLightSpecular = 0;
    GLint m_uniformMap0 = 0;
    GLint m_uniformHighlight = 0;

 public:
    MainShader();
    ~MainShader() override;

    bool load();

    void setLight(float* position, float* ambient, float* diffuse, float* specular) const;
    void setMatrices(const Geek::Matrix4& modelView, const Geek::Matrix4& viewProjection, const Geek::Matrix4& matrixNormal, bool highlight) const;
};

class SkyShader : public ShaderProgram
{
 private:
    GLint m_uniformMatrixModelView = 0;
    GLint m_uniformMatrixViewProjection = 0;
    GLint m_time = 0;

 public:
    SkyShader();
    ~SkyShader() override;

    bool load();
    void setTime(float time) const;
    void setMatrices(const Geek::Matrix4& modelView, const Geek::Matrix4& viewProjection) const;
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
