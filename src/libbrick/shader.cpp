//
//

#include "shader.h"

#include <geek/core-data.h>

using namespace std;
using namespace Geek;

ShaderProgram::ShaderProgram() : Logger("ShaderProgram")
{
    m_progId = glCreateProgram();
    log(DEBUG, "ShaderProgram: m_progId=%d", m_progId);
    CHECK_GL_ERROR("glCreateProgram");
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_progId);
}

bool ShaderProgram::load(GLenum type, const char* source)
{
    bool res;
    Data data;
    res = data.load(source);
    if (!res)
    {
        log(WARN, "load: Unable to load %s", source);
        return false;
    }

    // Create the GL shader
    GLuint id = glCreateShader(type);
    log(DEBUG, "load: id=%d", id);
    CHECK_GL_ERROR("glCreateShader");

    // Attach the source
    const char* dataPtr = data.getData();
    int length = (int)data.getLength();
    glShaderSource(id, 1, &dataPtr, &length);

    // Compile it
    GL(glCompileShader(id));

    // Check for errors
    const int MAX_LENGTH = 2048;
    char logText[MAX_LENGTH];
    int logLength = 0;
    int status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
        glGetShaderInfoLog(id, MAX_LENGTH, &logLength, logText);
        log(ERROR, "Failed to compile shader: %s", logText);
        return false;
    }

    // Attach to the shader program
    GL(glAttachShader(m_progId, id));

    return true;
}

bool ShaderProgram::link()
{
    // Link it...
    GL(glLinkProgram(m_progId));

    use();

    // check GLSL status
    int linkStatus;
    GL(glGetProgramiv(m_progId, GL_LINK_STATUS, &linkStatus));
    if (linkStatus == GL_FALSE)
    {
        const int MAX_LENGTH = 2048;
        char logText[MAX_LENGTH];
        int logLength = 0;
        glGetProgramiv(m_progId, GL_INFO_LOG_LENGTH, &logLength);
        glGetProgramInfoLog(m_progId, MAX_LENGTH, &logLength, logText);
        log(ERROR, "link: %s", logText);
        return false;
    }

    return true;
}

void ShaderProgram::use() const
{
    GL(glUseProgram(m_progId));
}

GLint ShaderProgram::getUniformLocation(const GLchar* name)
{
    use();
    GLint res = glGetUniformLocation(m_progId, name);
    log(DEBUG, "getUniformLocation: %s -> %d", name, res);
    CHECK_GL_ERROR("glGetUniformLocation");
    return res;
}

void ShaderProgram::unuse() const
{
    GL(glUseProgram(0));
}

OverlayShader::OverlayShader()
{

}

OverlayShader::~OverlayShader()
{

}

bool OverlayShader::load()
{
    bool res = ShaderProgram::load(GL_VERTEX_SHADER, "../data/shaders/ortho2D_vert.glsl");
    if (!res)
    {
        return false;
    }

    res = ShaderProgram::load(GL_FRAGMENT_SHADER, "../data/shaders/ortho2D_frag.glsl");
    if (!res)
    {
        return false;
    }

    res = link();
    if (!res)
    {
        return false;
    }

    m_uniformPositionX = getUniformLocation("positionX");
    m_uniformPositionY = getUniformLocation("positionY");
    m_uniformWidth = getUniformLocation("width");
    m_uniformHeight = getUniformLocation("height");
    m_uniformTextureWidth = getUniformLocation("textureWidth");
    m_uniformTextureHeight = getUniformLocation("textureHeight");

    GLint sampler = getUniformLocation("map0");
    GL(glUniform1i(sampler, 0));

    return true;
}

void OverlayShader::set(float x, float y, float width, float height, float textureWidth, float texureHeight) const
{
    GL(glUniform1f(m_uniformPositionX, x));
    GL(glUniform1f(m_uniformPositionY, y));
    GL(glUniform1f(m_uniformWidth, width));
    GL(glUniform1f(m_uniformHeight, height));
    GL(glUniform1f(m_uniformTextureWidth, textureWidth));
    GL(glUniform1f(m_uniformTextureHeight, texureHeight));
}
