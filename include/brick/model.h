//
// Created by Ian Parker on 07/12/2022.
//

#ifndef BLOCKY_MODEL_H
#define BLOCKY_MODEL_H

class Model
{
 public:
    [[nodiscard]] virtual void* getPositionPtr() const = 0;
    [[nodiscard]] virtual void* getNormalPtr() const = 0;
    [[nodiscard]] virtual void* getTexCoordPtr() const = 0;

    virtual bool init() = 0;

    virtual void bind() const = 0;

    virtual void draw() const;
};

#endif //BLOCKY_MODEL_H
