//
// Created by Ian Parker on 23/09/2022.
//

#ifndef BLOCKY_SPHEREMODEL_H
#define BLOCKY_SPHEREMODEL_H

#include "brick/engine.h"
#include "model.h"

#include <vector>

class SphereModel : public BlockyModel
{
 private:
    float radius = 0.5f;
    int sectorCount = 36;                        // longitude, # of slices
    int stackCount = 18;                         // latitude, # of stacks
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;
    std::vector<unsigned int> lineIndices;

    GLuint m_vbo = 0;
    GLuint m_ibo = 0;
    GLuint m_vao = 0;

    void addVertex(float x, float y, float z);
    void addNormal(float x, float y, float z);
    void addTexCoord(float s, float t);
    void addIndices(unsigned int i1, unsigned int i2, unsigned int i3);

    std::vector<float> computeFaceNormal(
        float x1, float y1, float z1,
        float x2, float y2, float z2,
        float x3, float y3, float z3);

 public:
    bool init() override;

    void bind() const override;
    void draw() const override;

    Side hit(const Geek::Vector& pos, Ray* ray) override;

    [[nodiscard]] void* getPositionPtr() const override;
    [[nodiscard]] void* getNormalPtr() const override;
    [[nodiscard]] void* getTexCoordPtr() const override;
};


#endif //BLOCKY_SPHEREMODEL_H
