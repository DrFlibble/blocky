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

#include <frontier/app.h>

#include "blocky/models/blockmodel.h"
#include "brick/engine.h"
#include "player.h"
#include "blocky/models/spheremodel.h"
#include "spheremob.h"

class World;
class Texture;
class Chunk;
class Block;
class BlockyShader;
class SkyShader;
class OverlayShader;
class BlockModel;
class Overlay;

class CrossHairOverlay;
class InfoOverlay;
class StatusBarOverlay;
class MenuOverlay;

class BlockyApp : public Frontier::FrontierApp
{
 private:
 public:
    BlockyApp();
    ~BlockyApp() = default;
};

class Blocky : public BrickEngine
{
 private:
    BlockyApp* m_blockyApp;

    BlockyModel* m_blockModel = nullptr;

    bool m_moveMode = true;
    Block* m_lookingAt = nullptr;
    Side m_lookingAtSide = NONE;
    SphereMob* m_lookingAtMob = nullptr;
    Geek::Vector m_lookingAtPos;

    BlockyShader* m_mainProgram = nullptr;
    SkyShader* m_skyProgram = nullptr;

    Texture* m_dirtTexture = nullptr;
    Texture* m_grassTexture = nullptr;
    Texture* m_stoneTexture = nullptr;
    Texture* m_targetTexture = nullptr;
    Texture* m_sphereTexture = nullptr;

    CrossHairOverlay* m_crossHairOverlay = nullptr;
    InfoOverlay* m_infoOverlay = nullptr;
    StatusBarOverlay* m_statusBarOverlay = nullptr;
    MenuOverlay* m_menuOverlay = nullptr;

    World* m_world = nullptr;
    SphereModel* m_sphereModel = nullptr;

    GLuint m_skyVAO = 0;

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

    BlockyApp* getApp() const { return m_blockyApp; }
    World* getWorld() const { return m_world; }

    Block* getLookingAt() { return m_lookingAt; }
    Geek::Vector getLookingAtPos() { return m_lookingAtPos; }

    void drawMobs();

    void breakBlock();
    void placeBlock();
};

#endif //BLOCKY_BLOCKY_H
