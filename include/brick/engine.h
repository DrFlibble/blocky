//
//

#ifndef BRICK_ENGINE_H
#define BRICK_ENGINE_H

#ifdef __APPLE__
#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED 1
#include <OpenGL/OpenGLAvailability.h>
#include <OpenGL/gl.h>
#include <OpenGL/gl3.h>
#else

#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1

#include <GL/gl.h>
#endif

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

#ifdef DEBUG_OPENGL
static void checkGLError(const char* function, int line, const char* msg)
{
    GLenum err = glGetError();
    if (err != 0)
    {
        printf("checkGLError: %s (%d): %d: %s", function, line, err, msg);
        abort();
    }
}
#endif

#define STRINGIFY(x) XSTRINGIFY(x)
#define XSTRINGIFY(x) #x

//#define DEBUG_OPENGL
#ifdef DEBUG_OPENGL
#define GL(__cmd) {__cmd;  checkGLError(__PRETTY_FUNCTION__, __LINE__, STRINGIFY(__cmd)); }
#define GLRES(__cmd) { auto __r = __cmd;  checkGLError(__PRETTY_FUNCTION__, __LINE__, STRINGIFY(__cmd)); __r; }
#define CHECK_GL_ERROR(__msg) checkGLError(__PRETTY_FUNCTION__, __LINE__, __msg);
#else
#define GL(__cmd) __cmd
#define CHECK_GL_ERROR(__msg) {}
#endif

class Overlay;
class OverlayShader;

class BrickEngine : protected Geek::Logger
{
 private:
    int m_screenWidth;
    int m_screenHeight;

    SDL_Window* m_window = nullptr;
    SDL_GLContext m_context = nullptr;

    Geek::FontManager* m_fontManager = nullptr;
    Geek::FontHandle* m_font = nullptr;

    OverlayShader* m_overlayProgram = nullptr;
    GLuint m_overlayVAO = 0;
    std::vector<Overlay*> m_overlays;

    SDL_GameController* m_controller = nullptr;

 protected:

    Geek::Matrix4 m_matrixModelView;
    Geek::Matrix4 m_matrixProjection;

    virtual bool initShaders() = 0;
    virtual bool initGame() = 0;
    virtual bool handleEvent(SDL_Event* event) = 0;
    virtual void update() = 0;
    virtual void drawFrame() = 0;
    virtual void exit() {}

    void captureMouse();
    void releaseMouse();

 public:
    explicit BrickEngine(const std::string& name);
    virtual ~BrickEngine();

    bool init();

    void resize(int width, int height);

    void mainLoop();

    void frame();

    Geek::FontManager* getFontManager() { return m_fontManager; }
    Geek::FontHandle* getFont() { return m_font; }
    SDL_GameController* getGameController() { return m_controller; }

    int getWidth() const { return m_screenWidth; }
    int getHeight() const { return m_screenHeight; }

    void addOverlay(Overlay* overlay)
    {
        m_overlays.push_back(overlay);
    }

    void drawOverlays();

    [[nodiscard]] GLuint getOverlayVAO() const { return m_overlayVAO; }
    [[nodiscard]] OverlayShader* getOverlayShader() const { return m_overlayProgram; }

};

#endif //BRICK_ENGINE_H
