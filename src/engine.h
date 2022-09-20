//
//

#ifndef BLOCKY_ENGINE_H
#define BLOCKY_ENGINE_H
#include <OpenGL/gl3.h>
#include <cstdio>
#include <cstdlib>
#include <SDL.h>
#include <SDL_video.h>

#include <string>

#include <geek/core-logger.h>
#include <geek/core-maths.h>
#include <geek/core-matrix.h>
#include <geek/fonts.h>

class Texture;

static void checkGLError(const char* function, int line, const char* msg)
{
    GLenum err = glGetError();
    if (err != 0)
    {
        printf("checkGLError: %s (%d): %d: %s", function, line, err, msg);
        abort();
    }
}

#define STRINGIFY(x) XSTRINGIFY(x)
#define XSTRINGIFY(x) #x
#define CHECK_GL_ERROR(__msg) checkGLError(__PRETTY_FUNCTION__, __LINE__, __msg);

//#define DEBUG_OPENGL
#ifdef DEBUG_OPENGL
#define GL(__cmd) {__cmd;  checkGLError(__PRETTY_FUNCTION__, __LINE__, STRINGIFY(__cmd)); }
#define GLRES(__cmd) { auto __r = __cmd;  checkGLError(__PRETTY_FUNCTION__, __LINE__, STRINGIFY(__cmd)); __r; }
#else
#define GL(__cmd) __cmd
#endif

class BlockyEngine : protected Geek::Logger
{
 protected:
    int m_screenWidth;
    int m_screenHeight;
    bool m_moveMode = true;

    SDL_Window* m_window = nullptr;
    SDL_GLContext m_context = nullptr;

    Geek::Matrix4 m_matrixModelView;
    Geek::Matrix4 m_matrixProjection;

    Geek::FontManager* m_fontManager = nullptr;
    Geek::FontHandle* m_font = nullptr;

    virtual bool initShaders() = 0;
    virtual bool initGame() = 0;
    virtual bool handleEvent(SDL_Event* event) = 0;
    virtual void update() = 0;
    virtual void drawFrame() = 0;
    virtual void exit() {}

    void captureMouse();
    void releaseMouse();

 public:
    explicit BlockyEngine(const std::string& name);
    virtual ~BlockyEngine();

    bool init();

    void resize(int width, int height);

    void mainLoop();

    void frame();

    Geek::FontManager* getFontManager() { return m_fontManager; }
    Geek::FontHandle* getFont() { return m_font; }
};

#endif //BLOCKY_ENGINE_H
