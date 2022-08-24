#ifndef BLOCKY_BLOCKY_H
#define BLOCKY_BLOCKY_H

#include <OpenGL/gl3.h>
#include <cstdio>
#include <cstdlib>
#include <SDL.h>
#include <SDL_video.h>

#include <geek/core-logger.h>
#include <geek/core-maths.h>
#include <geek/core-matrix.h>
#include <geek/gfx-surface.h>

#include "blockmodel.h"
#include "engine.h"

class World;
class Texture;
class Chunk;
class Block;
class MainShader;
class SkyShader;
class OverlayShader;
class BlockModel;

class Blocky : public BlockyEngine
{
 private:
    Model* m_blockModel = nullptr;
    Geek::Vector m_player;

    float m_heading = 0;
    float m_pitch = 0;
    float m_playerHeight = 1.8;

    Block* m_lookingAt = nullptr;
    Side m_lookingAtSide = NONE;
    Geek::Vector m_lookingAtPos;

    MainShader* m_mainProgram = nullptr;
    SkyShader* m_skyProgram = nullptr;
    OverlayShader* m_overlayProgram = nullptr;

    Texture* m_dirtTexture = nullptr;
    Texture* m_grassTexture = nullptr;
    Texture* m_targetTexture = nullptr;

    Geek::Gfx::Surface* m_overlaySurface = nullptr;
    Texture* m_overlayTexture = nullptr;

    World* m_world = nullptr;

    GLuint m_skyVAO = 0;

    GLuint m_overlayIBO = 0;
    GLuint m_overlayVBO = 0;
    GLuint m_overlayVAO = 0;

    float m_lastMillis = 0;

    // Movement
    float m_strafe = 0;
    float m_forward = 0;

    bool initShaders() override;
    bool initGame() override;

    void drawChunk(Chunk* chunk);
    void drawBox(const Geek::Matrix4& matrix, bool highlight);

    void calcLookAt();
 public:
    Blocky();
    ~Blocky() override;

    void update() override;
    void drawFrame() override;

    bool handleEvent(SDL_Event* event) override;
};

#endif //BLOCKY_BLOCKY_H
