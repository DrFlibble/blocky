#ifndef BLOCKY_BLOCKMODEL_H
#define BLOCKY_BLOCKMODEL_H

#include <OpenGL/gl3.h>

class BlockModel
{
 private:
    GLuint m_cubeVBO = 0;
    GLuint m_cubeIBO = 0;

 public:
    BlockModel();
    ~BlockModel();

    bool init();

    void bind() const;

    [[nodiscard]] void* getPositionPtr() const;
    [[nodiscard]] void* getNormalPtr() const;
    [[nodiscard]] void* getTexCoordPtr() const;
};

#endif //BLOCKY_BLOCKMODEL_H
