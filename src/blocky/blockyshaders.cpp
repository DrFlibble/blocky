//
//

#include "blockyshaders.h"

using namespace std;
using namespace Geek;

MainShader::MainShader() = default;

MainShader::~MainShader() = default;

bool MainShader::load()
{
    bool res;

    res = ShaderProgram::load(GL_VERTEX_SHADER, "../data/shaders/main_vertex.glsl");
    if (!res)
    {
        return false;
    }

    res = ShaderProgram::load(GL_FRAGMENT_SHADER, "../data/shaders/main_fragment.glsl");
    if (!res)
    {
        return false;
    }

    res = link();
    if (!res)
    {
        return false;
    }

    m_uniformMatrixModelView = getUniformLocation("matrixModelView");
    m_uniformMatrixModelViewProjection = getUniformLocation("matrixModelViewProjection");
    m_uniformMatrixNormal = getUniformLocation("matrixNormal");
    m_uniformLightPosition = getUniformLocation("lightPosition");
    m_uniformLightAmbient = getUniformLocation("lightAmbient");
    m_uniformLightDiffuse = getUniformLocation("lightDiffuse");
    m_uniformLightSpecular = getUniformLocation("lightSpecular");
    m_uniformMap0 = getUniformLocation("map0");
    m_uniformHighlight = getUniformLocation("highlight");

    GL(glUniform1i(m_uniformMap0, 0));

    return true;
}

void MainShader::setLight(float* position, float* ambient, float* diffuse, float* specular) const
{
    GL(glUniform4fv(m_uniformLightPosition, 1, position));
    GL(glUniform4fv(m_uniformLightAmbient, 1, ambient));
    GL(glUniform4fv(m_uniformLightDiffuse, 1, diffuse));
    GL(glUniform4fv(m_uniformLightSpecular, 1, specular));
}

void MainShader::setMatrices(const Geek::Matrix4& modelView, const  Geek::Matrix4& viewProjection, const Geek::Matrix4& matrixNormal, bool highlight) const
{
    GL(glUniformMatrix4fv(m_uniformMatrixModelView, 1, false, modelView.get()));
    GL(glUniformMatrix4fv(m_uniformMatrixModelViewProjection, 1, false, viewProjection.get()));
    GL(glUniformMatrix4fv(m_uniformMatrixNormal, 1, false, matrixNormal.get()));
    GL(glUniform1i(m_uniformHighlight, highlight));
}

SkyShader::SkyShader() = default;
SkyShader::~SkyShader() = default;

bool SkyShader::load()
{
    bool res;

    res = ShaderProgram::load(GL_VERTEX_SHADER, "../data/shaders/sky_vertex.glsl");
    if (!res)
    {
        return false;
    }

    res = ShaderProgram::load(GL_FRAGMENT_SHADER, "../data/shaders/sky_fragment.glsl");
    if (!res)
    {
        return false;
    }

    res = link();
    if (!res)
    {
        return false;
    }

    m_uniformMatrixModelView = getUniformLocation("viewMatrix");
    m_uniformMatrixViewProjection = getUniformLocation("projectionMatrix");
    m_time = getUniformLocation("time");

    return true;
}

void SkyShader::setMatrices(const Matrix4 &modelView, const Matrix4 &viewProjection) const
{
    GL(glUniformMatrix4fv(m_uniformMatrixModelView, 1, false, modelView.get()));
    GL(glUniformMatrix4fv(m_uniformMatrixViewProjection, 1, false, viewProjection.get()));
}

void SkyShader::setTime(float time) const
{
    glUniform1f(m_time, time);
}
