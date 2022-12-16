//
// Created by Ian Parker on 14/12/2022.
//

#ifndef BLOCKY_BOTT_H
#define BLOCKY_BOTT_H

#include <brick/engine.h>
#include <brick/texture.h>
#include <brick/baseentity.h>

#include "player.h"

class SimpleShader;
class GrassShader;
class Terrain;

class BottEngine : public BrickEngine
{
 private:
    SimpleShader* m_mainShader = nullptr;
    GrassShader* m_grassShader = nullptr;

    Terrain* m_terrain = nullptr;
    Texture* m_grassTexture = nullptr;
    Texture* m_grassFlowTexture = nullptr;

    Player m_player;

 public:
    BottEngine();
    ~BottEngine() override;

    bool initShaders() override;
    bool initGame() override;
    bool handleEvent(SDL_Event* event) override;
    void update() override;
    void drawFrame() override;

    Terrain* getTerrain() { return m_terrain; }
    GrassShader* getGrassShader() { return m_grassShader; }
    Texture* getGrassTexture() { return m_grassTexture; }
    Texture* getGrassFlowTexture() { return m_grassFlowTexture; }

    Player& getPlayer() { return m_player; }
};

#endif //BLOCKY_BOTT_H
