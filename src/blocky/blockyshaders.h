//
//

#ifndef BLOCKY_BLOCKYSHADERS_H
#define BLOCKY_BLOCKYSHADERS_H

#include "blocky.h"
#include "brick/shader.h"

#include <geek/core-logger.h>

class BlockyShader : public ShaderProgram
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
    BlockyShader();
    ~BlockyShader() override;

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

#endif //BLOCKY_BLOCKYSHADERS_H
