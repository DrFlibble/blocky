//
// Created by Ian Parker on 07/12/2022.
//

#include "brick/engine.h"
#include "brick/model.h"

class BottEngine : public BlockyEngine
{

 public:
    BottEngine();
    ~BottEngine() override;

    bool initShaders() override;
    bool initGame() override;
    bool handleEvent(SDL_Event* event) override;
    void update() override;
    void drawFrame() override;
};

BottEngine::BottEngine() : BlockyEngine("BotT")
{

}

BottEngine::~BottEngine()
{

}

bool BottEngine::initShaders()
{
    return false;
}

bool BottEngine::initGame()
{
    return false;
}

bool BottEngine::handleEvent(SDL_Event* event)
{
    return false;
}

void BottEngine::update()
{

}

void BottEngine::drawFrame()
{

}

int main(int argc, char** argv)
{
    BottEngine* bottEngine = new BottEngine();
    bottEngine->init();

    bottEngine->mainLoop();

    return 0;
}

