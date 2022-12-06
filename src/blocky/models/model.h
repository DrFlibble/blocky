#ifndef __BLOCKY_MODELS_MODEL_H_
#define __BLOCKY_MODELS_MODEL_H_

#include "libbrick/ray.h"

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

    virtual void draw() const;
};

#endif
