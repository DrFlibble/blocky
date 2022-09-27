#ifndef BLOCKY_BLOCKY_H
#define BLOCKY_BLOCKY_H

#include <cstdio>
#include <cstdlib>
#include <SDL.h>
#include <SDL_video.h>

#include <geek/core-logger.h>
#include <geek/core-maths.h>
#include <geek/core-matrix.h>
#include <geek/gfx-surface.h>

#include "models/blockmodel.h"
#include "engine.h"
#include "player.h"
#include "overlays/crosshair.h"
#include "overlays/infooverlay.h"
#include "overlays/statusbar.h"
#include "models/spheremodel.h"
#include "spheremob.h"

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
    SphereMob* m_lookingAtMob = nullptr;
    Geek::Vector m_lookingAtPos;

    MainShader* m_mainProgram = nullptr;
    SkyShader* m_skyProgram = nullptr;
    OverlayShader* m_overlayProgram = nullptr;

    Texture* m_dirtTexture = nullptr;
    Texture* m_grassTexture = nullptr;
    Texture* m_stoneTexture = nullptr;
    Texture* m_targetTexture = nullptr;
    Texture* m_sphereTexture = nullptr;

    CrossHairOverlay* m_crossHairOverlay = nullptr;
    InfoOverlay* m_infoOverlay = nullptr;
    StatusBarOverlay* m_statusBarOverlay = nullptr;

    World* m_world = nullptr;
    SphereModel* m_sphereModel = nullptr;

    GLuint m_skyVAO = 0;
    GLuint m_overlayVAO = 0;

    float m_controllerForward = 0.0;
    float m_controllerStrafe = 0.0;
    float m_controllerHeading = 0.0;
    float m_controllerPitch = 0.0;

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

    World* getWorld() const { return m_world; }

    Block* getLookingAt() { return m_lookingAt; }
    Geek::Vector getLookingAtPos() { return m_lookingAtPos; }

    void drawMobs();

    void breakBlock();
    void placeBlock();
};

#endif //BLOCKY_BLOCKY_H
