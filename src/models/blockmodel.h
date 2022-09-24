#ifndef BLOCKY_BLOCKMODEL_H
#define BLOCKY_BLOCKMODEL_H

#include "engine.h"
#include "models/model.h"

#include <geek/core-maths.h>

class Ray;

class BlockModel : public Model
{
 private:
    GLuint m_cubeVBO = 0;
    GLuint m_cubeIBO = 0;
    GLuint m_cubeVAO = 0;

    static bool hitTriangle(Geek::Vector* vertices, const Geek::Vector& pos, Ray* ray, Hit& hit);
 public:
    BlockModel();
    ~BlockModel();

    bool init() override;

    void bind() const override;

    [[nodiscard]] void* getPositionPtr() const override;
    [[nodiscard]] void* getNormalPtr() const override;
    [[nodiscard]] void* getTexCoordPtr() const override;

    Side hit(const Geek::Vector& pos, Ray* ray) override;

};

#endif //BLOCKY_BLOCKMODEL_H
