//
// Created by Ian Parker on 12/12/2022.
//

#ifndef BLOCKY_SIMPLESHADER_H
#define BLOCKY_SIMPLESHADER_H

#include "brick/shader.h"

class SimpleShader : public ShaderProgram
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
    //GLint m_uniformHighlight = 0;

 public:
    SimpleShader();
    ~SimpleShader() override;

    bool load();

    void setLight(float* position, float* ambient, float* diffuse, float* specular) const;
    void setMatrices(const Geek::Matrix4& modelView, const Geek::Matrix4& viewProjection, const Geek::Matrix4& matrixNormal, bool highlight) const;
};


#endif //BLOCKY_SIMPLESHADER_H
