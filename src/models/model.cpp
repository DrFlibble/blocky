
#include "models/model.h"
#include "engine.h"

void Model::draw() const
{
    // activate attribs
    GL(glEnableVertexAttribArray(0));
    GL(glEnableVertexAttribArray(1));
    GL(glEnableVertexAttribArray(2));

    GL(glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, getPositionPtr()));
    GL(glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, getNormalPtr()));
    GL(glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, getTexCoordPtr()));

    GL(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr));

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);//attribVertexNormal);
    glDisableVertexAttribArray(2);//attribVertexTexCoord);

}
