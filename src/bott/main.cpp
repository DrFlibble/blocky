//
// Created by Ian Parker on 07/12/2022.
//

#include "brick/engine.h"
#include "brick/texture.h"
#include "brick/baseentity.h"
#include "simpleshader.h"
#include "grassshader.h"
#include "terrain.h"
#include "bott.h"

class SimpleShader;
class TerrainChunk;
class BottEngine;

using namespace std;
using namespace Geek::Gfx;

int main(int argc, char** argv)
{
    auto bottEngine = new BottEngine();
    bottEngine->init();

    bottEngine->mainLoop();

    delete bottEngine;

    return 0;
}

