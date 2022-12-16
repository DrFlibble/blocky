//
// Created by Ian Parker on 13/12/2022.
//

#ifndef BLOCKY_GRASSSHADER_H
#define BLOCKY_GRASSSHADER_H

#include <brick/shader.h>

class GrassShader : public ShaderProgram
{
 private:
    GLint m_uniformView{};
    GLint m_uniformCameraPosition{};
    GLint m_uniformTime{};
    GLint m_uniformProjection{};
    GLint m_uniformTexture1{};
    GLint m_uniformWind{};

 public:
    GrassShader() = default;

    bool load();

    void setMatrices(const Geek::Vector& cameraPosition, const Geek::Matrix4& projectionView, const Geek::Matrix4& viewProjection) const;
};


#endif //BLOCKY_GRASSSHADER_H
