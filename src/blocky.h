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
#include "player.h"

class World;
class Texture;
class Chunk;
class Block;
class MainShader;
class SkyShader;
class OverlayShader;
class BlockModel;
class Overlay;

class Blocky : public BlockyEngine
{
 private:
    Model* m_blockModel = nullptr;


    Block* m_lookingAt = nullptr;
    Side m_lookingAtSide = NONE;
    Geek::Vector m_lookingAtPos;

    MainShader* m_mainProgram = nullptr;
    SkyShader* m_skyProgram = nullptr;
    OverlayShader* m_overlayProgram = nullptr;

    Geek::Gfx::Surface* m_dirtIcon = nullptr;
    Texture* m_dirtTexture = nullptr;
    Geek::Gfx::Surface* m_grassIcon = nullptr;
    Texture* m_grassTexture = nullptr;
    Geek::Gfx::Surface* m_stoneIcon = nullptr;
    Texture* m_stoneTexture = nullptr;
    Texture* m_targetTexture = nullptr;

    Overlay* m_crossHairOverlay = nullptr;
    Overlay* m_infoOverlay = nullptr;
    Overlay* m_statusBarOverlay = nullptr;

    Geek::Gfx::Surface* m_heartIcon = nullptr;
    Geek::Gfx::Surface* m_shieldIcon = nullptr;

    World* m_world = nullptr;

    GLuint m_skyVAO = 0;
    GLuint m_overlayVAO = 0;

    float m_lastMillis = 0;


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
    void exit() override;

    bool handleEvent(SDL_Event* event) override;

    int getWidth() { return m_screenWidth; }
    int getHeight() { return m_screenHeight; }
    [[nodiscard]] GLuint getOverlayVAO() const { return m_overlayVAO; }
    [[nodiscard]] OverlayShader* getOverlayShader() const { return m_overlayProgram; }
};

#endif //BLOCKY_BLOCKY_H
