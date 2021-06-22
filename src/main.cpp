
#include <array>

#include <OpenGL/gl3.h>

#include <SDL.h>
#include <SDL_video.h>

#include <geek/core-logger.h>
#include <geek/core-maths.h>
#include <geek/core-matrix.h>
#include <geek/core-data.h>

#include "texture.h"
#include "blockmodel.h"
#include "world.h"

using namespace std;
using namespace Geek;

class Blocky : private Geek::Logger
{
 private:
    SDL_Window* m_window = nullptr;
    SDL_GLContext m_context = nullptr;

    BlockModel* m_blockModel = nullptr;
    Geek::Vector m_player;

    float m_heading = 0;
    float m_pitch = 0;

    Matrix4 matrixModelView;
    Matrix4 matrixProjection;

    GLuint m_progId = 0;
    GLint uniformMatrixModelView = 0;
    GLint uniformMatrixModelViewProjection = 0;
    GLint uniformMatrixNormal = 0;
    GLint uniformLightPosition = 0;
    GLint uniformLightAmbient = 0;
    GLint uniformLightDiffuse = 0;
    GLint uniformLightSpecular = 0;
    GLint uniformMap0 = 0;
    GLint attribVertexPosition = 0;
    GLint attribVertexNormal = 0;
    GLint attribVertexTexCoord = 0;

    Texture* m_dirtTexture = nullptr;
    Texture* m_grassTexture = nullptr;

    World* m_world = nullptr;

    bool initShaders();

 public:
    Blocky();
    ~Blocky();

    bool init();

    void display();

    void resize(int width, int height);

    void drawBox(const Matrix4& matrix) const;

    void mainLoop();

    void drawChunk(Chunk* chunk, const Matrix4& matrixView);
};

Blocky::Blocky() : Logger("Blocky")
{
    m_player.set(0, 16.0, 0);
    m_world = new World();
}

Blocky::~Blocky()
{
    SDL_GL_DeleteContext(m_context);
    SDL_Quit();
}

bool Blocky::init()
{
    int res;
    res = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    if (res < 0)
    {
        log(ERROR, "Failed to initialise SDL!");
        return false;
    }

    m_window = SDL_CreateWindow("Blocky", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
    m_context = SDL_GL_CreateContext(m_window);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // 4-byte pixel alignment
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    // enable /disable features
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);

    // track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
    glEnable(GL_TEXTURE);

    // background color
    glClearColor(0, 0, 0, 0);
    // clear stencil buffer
    glClearStencil(0);
    // 0 is near, 1 is far
    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);

    res = initShaders();
    if (!res)
    {
        return false;
    }

    m_blockModel = new BlockModel();
    res = m_blockModel->init();
    if (!res)
    {
        return false;
    }

    Geek::Gfx::Surface* dirtSurface = Geek::Gfx::Surface::loadPNG("../dirt-512.png");
    m_dirtTexture = new Texture(dirtSurface);

    Geek::Gfx::Surface* grassSurface = Geek::Gfx::Surface::loadPNG("../grass-512.png");
    m_grassTexture = new Texture(grassSurface);

    m_world->generate();

    return true;
}

bool Blocky::initShaders()
{
    // create shader and program
    GLuint vsId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fsId = glCreateShader(GL_FRAGMENT_SHADER);
    m_progId = glCreateProgram();

    bool res;
    Data fsData;
    res = fsData.load("../data/shaders/main.fs");
    if (!res)
    {
        log(WARN, "initShaders: Unable to load main.fs");
        return false;
    }
    Data vsData;
    res = vsData.load("../data/shaders/main.vs");
    if (!res)
    {
        log(WARN, "initShaders: Unable to load main.vs");
        return false;
    }

    // load shader sources
    const char* data = vsData.getData();
    int length = (int)vsData.getLength();
    glShaderSource(vsId, 1, &data, &length);

    data = fsData.getData();
    length = (int)fsData.getLength();
    glShaderSource(fsId, 1, &data, &length);

    // compile shader sources
    glCompileShader(vsId);
    glCompileShader(fsId);

    const int MAX_LENGTH = 2048;
    char logText[MAX_LENGTH];
    int logLength = 0;
    int vsStatus;
    int fsStatus;
    glGetShaderiv(vsId, GL_COMPILE_STATUS, &vsStatus);
    if (vsStatus == GL_FALSE)
    {
        glGetShaderiv(vsId, GL_INFO_LOG_LENGTH, &logLength);
        glGetShaderInfoLog(vsId, MAX_LENGTH, &logLength, logText);
        log(DEBUG, "===== Vertex Shader Log =====");
        log(DEBUG, "%s", logText);
        return false;
    }

    glGetShaderiv(fsId, GL_COMPILE_STATUS, &fsStatus);
    if (fsStatus == GL_FALSE)
    {
        glGetShaderiv(fsId, GL_INFO_LOG_LENGTH, &logLength);
        glGetShaderInfoLog(fsId, MAX_LENGTH, &logLength, logText);
        log(DEBUG, "===== Fragment Shader Log =====");
        log(DEBUG, "%s", logText);
        return false;
    }

    // attach shaders to the program
    glAttachShader(m_progId, vsId);
    glAttachShader(m_progId, fsId);

    // link program
    glLinkProgram(m_progId);

    // get uniform/attrib locations
    glUseProgram(m_progId);
    uniformMatrixModelView = glGetUniformLocation(m_progId, "matrixModelView");
    uniformMatrixModelViewProjection = glGetUniformLocation(m_progId, "matrixModelViewProjection");
    uniformMatrixNormal = glGetUniformLocation(m_progId, "matrixNormal");
    uniformLightPosition = glGetUniformLocation(m_progId, "lightPosition");
    uniformLightAmbient = glGetUniformLocation(m_progId, "lightAmbient");
    uniformLightDiffuse = glGetUniformLocation(m_progId, "lightDiffuse");
    uniformLightSpecular = glGetUniformLocation(m_progId, "lightSpecular");
    uniformMap0 = glGetUniformLocation(m_progId, "map0");
    attribVertexPosition = glGetAttribLocation(m_progId, "vertexPosition");
    attribVertexNormal = glGetAttribLocation(m_progId, "vertexNormal");
    attribVertexTexCoord = glGetAttribLocation(m_progId, "vertexTexCoord");

    // set uniform values
    float lightPosition[] = {0, 0, 1, 0};
    float lightAmbient[] = {0.3f, 0.3f, 0.3f, 1};
    float lightDiffuse[] = {0.7f, 0.7f, 0.7f, 1};
    float lightSpecular[] = {1.0f, 1.0f, 1.0f, 1};
    glUniform4fv(uniformLightPosition, 1, lightPosition);
    glUniform4fv(uniformLightAmbient, 1, lightAmbient);
    glUniform4fv(uniformLightDiffuse, 1, lightDiffuse);
    glUniform4fv(uniformLightSpecular, 1, lightSpecular);
    glUniform1i(uniformMap0, 0);

    // unbind GLSL
    glUseProgram(0);

    // check GLSL status
    int linkStatus;
    glGetProgramiv(m_progId, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE)
    {
        glGetProgramiv(m_progId, GL_INFO_LOG_LENGTH, &logLength);
        glGetProgramInfoLog(m_progId, MAX_LENGTH, &logLength, logText);
        log(DEBUG, "===== GLSL Program Log =====");
        log(DEBUG, "%s", logText);
        return false;
    }

    return true;
}

void Blocky::resize(int width, int height)
{
    // Setup our viewport
    glViewport(0, 0, width, height);

    const float N = 0.1f;
    const float F = 100.0f;
    const float FOV_Y = 60.0f * DEG2RAD;

    // construct perspective projection matrix
    float aspectRatio = (float)(width) / (float)height;
    float tangent = tanf(FOV_Y / 2.0f);     // tangent of half fovY
    float h = N * tangent;                  // half height of near plane
    float w = h * aspectRatio;              // half width of near plane
    matrixProjection.identity();
    matrixProjection[0]  =  N / w;
    matrixProjection[5]  =  N / h;
    matrixProjection[10] = -(F + N) / (F - N);
    matrixProjection[11] = -1;
    matrixProjection[14] = -(2 * F * N) / (F - N);
    matrixProjection[15] =  0;
}

void Blocky::drawBox(const Matrix4& matrix) const
{
    glUseProgram(m_progId);

    // set matrix uniforms every frame
    Matrix4 matrixModelViewProjection = matrixProjection * matrix;
    Matrix4 matrixNormal = matrix;
    float c[] = {0, 0, 0, 1};
    matrixNormal.setColumn(3, c);
    glUniformMatrix4fv(uniformMatrixModelView, 1, false, matrix.get());
    glUniformMatrix4fv(uniformMatrixModelViewProjection, 1, false, matrixModelViewProjection.get());
    glUniformMatrix4fv(uniformMatrixNormal, 1, false, matrixNormal.get());

    m_blockModel->bind();

    // activate attribs
    glEnableVertexAttribArray(attribVertexPosition);
    glEnableVertexAttribArray(attribVertexNormal);
    glEnableVertexAttribArray(attribVertexTexCoord);

    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, false, 0, m_blockModel->getPositionPtr());
    glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, false, 0, m_blockModel->getNormalPtr());
    glVertexAttribPointer(attribVertexTexCoord, 2, GL_FLOAT, false, 0, m_blockModel->getTexCoordPtr());

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

    glDisableVertexAttribArray(attribVertexPosition);
    glDisableVertexAttribArray(attribVertexNormal);
    glDisableVertexAttribArray(attribVertexTexCoord);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glUseProgram(0);
}

void Blocky::display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    Matrix4 matrixView;
    //matrixView.rotateY(m_heading);   // heading
    //matrixView.rotateX(m_pitch);   // pitch

    Matrix4 matrixModel;

    // set modelview matrix
    matrixModelView = matrixView * matrixModel;

    int drawDistance = 2;
    int playerChunkX = CHUNK_NUM((int)m_player.x);
    int playerChunkZ = CHUNK_NUM((int)m_player.z);

    for (int x = -drawDistance; x <= drawDistance; x++)
    {
        for (int z = -drawDistance; z <= drawDistance; z++)
        {
            Chunk* chunk = m_world->getChunk(x + playerChunkX, z + playerChunkZ);
            drawChunk(chunk, matrixView);
        }
    }

    SDL_GL_SwapWindow(m_window);
}

void Blocky::drawChunk(Chunk* chunk, const Matrix4& matrixView)
{
    for (int chunkX = 0; chunkX < 16; chunkX++)
    {
        for (int chunkZ = 0; chunkZ < 16; chunkZ++)
        {
            for (int y = 0; y < 32; y++)
            {
                const Block* block = chunk->getBlock(chunkX, y, chunkZ);

                if (block != nullptr)
                {
                    float x = (float)(chunkX + chunk->getChunkX() * 16) - (float)m_player.x;
                    float z = (float)(chunkZ + chunk->getChunkZ() * 16) - (float)m_player.z;

                    if (block->getType() == DIRT)
                    {
                        m_dirtTexture->bind();
                    }
                    else
                    {
                        m_grassTexture->bind();
                    }
                    Matrix4 matrix = matrixView;
                    matrix.translate(x + 0.5f, (float)(y - m_player.y) + 0.5f, z + 0.5f);
                    matrix.rotateY(m_heading);   // heading
                    matrix.rotateX(m_pitch);   // pitch
                    drawBox(matrix);
                }
            }
        }
    }
}

void Blocky::mainLoop()
{
    resize(800, 600);

    SDL_ShowCursor(false);
    SDL_WarpMouseInWindow(m_window, 400, 300);
    float forward = 0;
    float strafe = 0;
    uint64_t lastMillis = SDL_GetTicks();

    while (true)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_w)
                {
                    forward = 0.2f;
                }
                else if (event.key.keysym.sym == SDLK_s)
                {
                    forward = -0.2f;
                }
                else if (event.key.keysym.sym == SDLK_a)
                {
                    strafe = -0.2f;
                }
                else if (event.key.keysym.sym == SDLK_d)
                {
                    strafe = 0.2f;
                }
            }
            else if (event.type == SDL_MOUSEMOTION)
            {
                m_heading += (float) (event.motion.x - 400) * 0.1f;
                m_pitch += (float) (event.motion.y - 300) * 0.1f;
                if (m_pitch > 90.0)
                {
                    m_pitch = 90.0;
                }
                else if (m_pitch < -90.0)
                {
                    m_pitch = -90.0;
                }
                SDL_WarpMouseInWindow(m_window, 400, 300);
            }
            else if (event.type == SDL_QUIT)
            {
                return;
            }
        }

        double headingSin = sin(m_heading * (M_PI / 180.0));
        double headingCos = cos(m_heading * (M_PI / 180.0));

        uint64_t time = SDL_GetTicks();
        if ((time - lastMillis) > (1000 / 100))
        {
            m_player.x += headingSin * forward + headingCos * strafe;
            m_player.z += headingCos * -forward + headingSin * strafe;
            forward *= 0.75f;
            strafe *= 0.5f;
            lastMillis = time;

#ifdef FALLING
            if (m_player.y > 2.20 && m_world->getBlock((int)m_player.x, (int)m_player.y - 2, (int)m_player.z) == nullptr)
            {
                m_player.y -= 0.20;
            }
#endif

            display();
        }
    }
}

int main(int argc, char** argv)
{
    Blocky blocky;

    bool res;
    res = blocky.init();
    if (!res)
    {
        return 0;
    }

    blocky.mainLoop();

    return 0;
}
