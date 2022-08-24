#ifndef BLOCKY_BLOCKMODEL_H
#define BLOCKY_BLOCKMODEL_H

#include <OpenGL/gl3.h>
#include <geek/core-maths.h>

class Ray;

enum Side
{
    NONE = -1,
    Z_POS = 0,
    X_POS,
    Y_POS = 2,
    X_NEG,
    Y_NEG = 4,
    Z_NEG = 5,
};

struct Hit
{
    int face = -1;
    float t = std::numeric_limits<float>::max();
};

class Model
{
 public:
    [[nodiscard]] virtual void* getPositionPtr() const = 0;
    [[nodiscard]] virtual void* getNormalPtr() const = 0;
    [[nodiscard]] virtual void* getTexCoordPtr() const = 0;

    virtual bool init() = 0;

    virtual void bind() const = 0;

    virtual Side hit(const Geek::Vector& pos, Ray* ray) = 0;

    void draw();
};

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
