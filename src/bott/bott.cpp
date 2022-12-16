//
// Created by Ian Parker on 14/12/2022.
//

#include "bott.h"

#include "simpleshader.h"
#include "grassshader.h"
#include "terrain.h"

using namespace Geek::Gfx;
using namespace Geek;

BottEngine::BottEngine() : BrickEngine("BotT"), m_player(this)
{
}

BottEngine::~BottEngine() = default;

bool BottEngine::initShaders()
{
    bool res;

    m_mainShader = new SimpleShader();
    res = m_mainShader->load();
    if (!res)
    {
        return false;
    }

    m_grassShader = new GrassShader();
    res = m_grassShader->load();
    if (!res)
    {
        return false;
    }

    return true;
}

bool BottEngine::initGame()
{
    m_terrain = new Terrain(this);
    bool res = m_terrain->init();
    if (!res)
    {
        return false;
    }

    Surface* grassSurface = Surface::loadPNG("../data/bott/textures/grass_texture.png");
    m_grassTexture = new Texture(grassSurface);
    Surface* grassFlowSurface = Surface::loadPNG("../data/bott/textures/flowmap.png");
    m_grassFlowTexture = new Texture(grassFlowSurface);

    int x = 16;
    int z = 16;
    m_player.getPosition().set(x, m_terrain->heightAt(x, z) + 1, z);
    return true;
}

bool BottEngine::handleEvent(SDL_Event* event)
{
    return m_player.handleEvent(event);
}

void BottEngine::update()
{
    m_player.update();
}

void BottEngine::drawFrame()
{
    m_matrixModelView = m_player.getMatrix();

    float lightPosition[] = {0, 1, 0, 0};
    float lightAmbient[] = {0.3f, 0.3f, 0.3f, 1};
    float lightDiffuse[] = {0.7f, 0.7f, 0.7f, 1};
    float lightSpecular[] = {1.0f, 1.0f, 1.0f, 1};

    m_terrain->bind();
    m_mainShader->use();
    m_mainShader->setLight(lightPosition, lightAmbient, lightDiffuse, lightSpecular);

    {
        Geek::Matrix4 matrix;
        //float offset = +0.5f;
        Geek::Vector position = m_player.getPosition();
        matrix.translate(-(position.x), -(position.y + 1), -(position.z));
        //matrix *= m_matrixModelView;
        //matrix *= rotateMatrix;
        matrix.rotateY(m_player.getHeading());
        matrix.rotateX(m_player.getPitch());

        // set matrix uniforms every drawFrame
        Geek::Matrix4 matrixModelViewProjection = m_matrixProjection * matrix;
        Geek::Matrix4 matrixNormal = matrix;

        matrixNormal.rotateY(m_player.getHeading());
        //matrix.rotateX(m_world->getPlayer().getPitch());   // pitch
        float c[] = {0, 0, 0, 1};
        matrixNormal.setColumn(3, c);

        m_mainShader->use();
        m_mainShader->setMatrices(matrix, matrixModelViewProjection, matrixNormal, false);

        m_grassShader->use();

        m_terrain->getEngine()->getGrassShader()->setMatrices(
            m_player.getPosition(),
            m_matrixProjection,
            matrix);
        m_mainShader->use();
    }

    m_terrain->draw();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //drawOverlays();

}
