//
//

#include "engine.h"

using namespace std;
using namespace Geek;

BlockyEngine::BlockyEngine(const string& name) : Logger("BlockyEngine[" + name + "]")
{
    m_screenWidth = 800;
    m_screenHeight = 600;
}

BlockyEngine::~BlockyEngine()
{
    if (m_context != nullptr)
    {
        SDL_GL_DeleteContext(m_context);
    }

    if (m_controller != nullptr)
    {
        SDL_GameControllerClose(m_controller);
    }
}

bool BlockyEngine::init()
{
    int res;
    res = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);
    if (res < 0)
    {
        log(ERROR, "Failed to initialise SDL!");
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

    m_window = SDL_CreateWindow(
        "Blocky",
        100,
        100,
        m_screenWidth,
        m_screenHeight,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE /*| SDL_WINDOW_MOUSE_GRABBED*/);
    m_context = SDL_GL_CreateContext(m_window);

    // 4-byte pixel alignment
    GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));

    // enable /disable features
    GL(glHint(GL_LINE_SMOOTH_HINT, GL_NICEST));
    GL(glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST));
    GL(glEnable(GL_DEPTH_TEST));
    GL(glEnable(GL_CULL_FACE));

    GL(glClearColor(0.0, 0.0, 0.0, 0));
    // clear stencil buffer
    GL(glClearStencil(0));
    // 0 is near, 1 is far
    GL(glClearDepth(1.0f));
    GL(glDepthFunc(GL_LEQUAL));

    res = initShaders();
    if (!res)
    {
        return false;
    }

    res = initGame();
    if (!res)
    {
        return false;
    }

    m_fontManager = new Geek::FontManager;
    m_fontManager->init();
    m_fontManager->scan("../data/fonts");
    m_font = m_fontManager->openFont("Lato", "Regular", 12);

    int numJoysticks = SDL_NumJoysticks();
    for (int i = 0; i < numJoysticks; i++)
    {
        if (SDL_IsGameController(i))
        {
            m_controller = SDL_GameControllerOpen(i);

            log(DEBUG, "init: Joystick %d: controller=%p", i, m_controller);
            break;
        }
    }


    return true;
}

void BlockyEngine::resize(int width, int height)
{
    m_screenWidth = width;
    m_screenHeight = height;

    // Setup our viewport
    GL(glViewport(0, 0, width, height));

    const float N = 0.1f;
    const float F = 100.0f;
    const float FOV_Y = 60.0f * DEG2RAD;

    // construct perspective projection matrix
    float aspectRatio = (float)(width) / (float)height;
    float tangent = tanf(FOV_Y / 2.0f);     // tangent of half fovY
    float h = N * tangent;                  // half height of near plane
    float w = h * aspectRatio;              // half width of near plane
    m_matrixProjection.identity();
    m_matrixProjection[0]  = N / w;
    m_matrixProjection[5]  = N / h;
    m_matrixProjection[10] = -(F + N) / (F - N);
    m_matrixProjection[11] = -1;
    m_matrixProjection[14] = -(2 * F * N) / (F - N);
    m_matrixProjection[15] =  0;
}

void BlockyEngine::mainLoop()
{
    resize(m_screenWidth, m_screenHeight);

    captureMouse();
    //SDL_WarpMouseInWindow(m_window, 400, 300);
    uint64_t fpsMillis = SDL_GetTicks();
    int frames = 0;

    while (true)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                exit();
                return;
            }
            else if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    resize(event.window.data1, event.window.data2);
                }
            }
            handleEvent(&event);
        }

        update();

        frame();

        frames++;
        uint64_t time = SDL_GetTicks();
        if ((time - fpsMillis) >= 1000)
        {
            log(DEBUG, "mainLoop: fps=%d", frames);
            fpsMillis = time;
            frames = 0;
        }
    }
}

void BlockyEngine::frame()
{
    GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

    drawFrame();

    GL(glUseProgram(0));

    SDL_GL_SwapWindow(m_window);
}

void BlockyEngine::captureMouse()
{
    SDL_WarpMouseInWindow(m_window, 0, 0);
    SDL_SetWindowInputFocus(m_window);
    SDL_CaptureMouse(SDL_TRUE);
    SDL_ShowCursor(SDL_FALSE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void BlockyEngine::releaseMouse()
{
    SDL_CaptureMouse(SDL_FALSE);
    SDL_ShowCursor(SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_FALSE);
}
