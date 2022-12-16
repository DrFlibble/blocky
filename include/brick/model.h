//
// Created by Ian Parker on 07/12/2022.
//

#ifndef BRICK_MODEL_H
#define BRICK_MODEL_H

class Model
{
 public:
    [[nodiscard]] virtual void* getPositionPtr() const { return nullptr; }
    [[nodiscard]] virtual void* getNormalPtr() const { return nullptr; }
    [[nodiscard]] virtual void* getTexCoordPtr() const { return nullptr; }

    virtual bool init() = 0;

    virtual void bind() const = 0;

    virtual void draw() const;
};

#endif //BRICK_MODEL_H
