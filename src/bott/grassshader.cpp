//
// Created by Ian Parker on 13/12/2022.
//

#include <sys/time.h>
#include "grassshader.h"

bool GrassShader::load()
{
    bool res;

    res = ShaderProgram::load(GL_VERTEX_SHADER, "../data/bott/shaders/grass_vertex.glsl");
    if (!res)
    {
        return false;
    }

    res = ShaderProgram::load(GL_FRAGMENT_SHADER, "../data/bott/shaders/grass_fragment.glsl");
    if (!res)
    {
        return false;
    }

    res = ShaderProgram::load(GL_GEOMETRY_SHADER, "../data/bott/shaders/grass_geometry.glsl");
    if (!res)
    {
        return false;
    }

    res = link();
    if (!res)
    {
        return false;
    }

    m_uniformView = getUniformLocation("u_view");
    m_uniformCameraPosition = getUniformLocation("u_cameraPosition");
    m_uniformTime = getUniformLocation("u_time");
    m_uniformProjection = getUniformLocation("u_projection");
    m_uniformTexture1 = getUniformLocation("u_textgrass");
    m_uniformWind = getUniformLocation("u_wind");

    return true;
}

void GrassShader::setMatrices(
    const Geek::Vector &cameraPosition,
    const Geek::Matrix4 &projectionView,
    const Geek::Matrix4 &viewMatrix) const
{
    use();
    GL(glUniformMatrix4fv(m_uniformProjection, 1, false, projectionView.get()));
    GL(glUniformMatrix4fv(m_uniformView, 1, false, viewMatrix.get()));

    float camera[] = {
        (float)cameraPosition.x,
        (float)cameraPosition.y,
        (float)cameraPosition.z};
    GL(glUniform3fv(m_uniformCameraPosition, 1, camera));

    float milliseconds = (float)SDL_GetTicks() / 1000.0f;
    glUniform1f(m_uniformTime, milliseconds);
    glUniform1i(m_uniformTexture1, 0);
    glUniform1i(m_uniformWind, 1);
}
