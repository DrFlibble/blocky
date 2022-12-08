#include "brick/texture.h"
#include "blocky/models/blockmodel.h"
#include "world.h"
#include "blockyshaders.h"
#include "blocky.h"
#include "brick/ray.h"
#include "entity.h"

#include "overlays/crosshair.h"
#include "overlays/infooverlay.h"
#include "overlays/menu.h"
#include "overlays/statusbar.h"

#include <frontier/engines/windowing.h>

using namespace std;
using namespace Geek;
using namespace Geek::Gfx;
using namespace Frontier;

Blocky::Blocky() : BlockyEngine("Blocky")
{
    m_blockyApp = new BlockyApp();
    m_world = new World();
}

Blocky::~Blocky() = default;

bool Blocky::initGame()
{
    if (!m_blockyApp->init())
    {
        return false;
    }

    glGenVertexArrays(1, &m_skyVAO);

    m_blockModel = new BlockModel();
    bool res = m_blockModel->init();
    if (!res)
    {
        return false;
    }

    Surface* dirtSurface = Surface::loadPNG("../dirt-512.png");
    Surface* grassSurface = Surface::loadPNG("../grassblock.png");
    Surface* stoneSurface = Surface::loadPNG("../stone.png");
    m_dirtTexture = new Texture(dirtSurface);
    m_grassTexture = new Texture(grassSurface);
    m_stoneTexture = new Texture(stoneSurface);
    m_targetTexture = new Texture("../target-512.png");
    m_sphereTexture = new Texture("../data/images/sphere.png");

    m_sphereModel = new SphereModel();
    m_sphereModel->init();

    m_crossHairOverlay = new CrossHairOverlay(this);
    m_infoOverlay = new InfoOverlay(this);
    m_statusBarOverlay = new StatusBarOverlay(this);
    m_menuOverlay = new MenuOverlay(this);
    m_menuOverlay->init();
    m_menuOverlay->setVisible(false);

    addOverlay(m_crossHairOverlay);
    addOverlay(m_infoOverlay);
    addOverlay(m_statusBarOverlay);
    addOverlay(m_menuOverlay);

    return true;
}

bool Blocky::initShaders()
{
    bool res;

    m_mainProgram = new MainShader();
    res = m_mainProgram->load();
    if (!res)
    {
        return false;
    }

    m_skyProgram = new SkyShader();
    res = m_skyProgram->load();
    if (!res)
    {
        return false;
    }

    m_skyProgram->unuse();


    return true;
}

void Blocky::drawBox(const Matrix4 &matrix, bool highlight)
{
    // set matrix uniforms every drawFrame
    Matrix4 matrixModelViewProjection = m_matrixProjection * matrix;
    Matrix4 matrixNormal = matrix;

    matrixNormal.rotateY(m_world->getPlayer().getHeading());   // heading
    //matrix.rotateX(m_world->getPlayer().getPitch());   // pitch
    float c[] = {0, 0, 0, 1};
    matrixNormal.setColumn(3, c);

    m_mainProgram->setMatrices(matrix, matrixModelViewProjection, matrixNormal, highlight);

    m_blockModel->draw();
}

void Blocky::drawFrame()
{
    m_matrixModelView = m_world->getPlayer().getMatrix();

    // Sky
    m_skyProgram->use();
    float time = (float) SDL_GetTicks() / 500.0f;
    m_skyProgram->setTime(time);
    m_skyProgram->setMatrices(m_matrixModelView, m_matrixProjection);
    glBindVertexArray(m_skyVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    int drawDistance = 2;
    int playerChunkX = CHUNK_NUM((int) m_world->getPlayer().getPosition().x);
    int playerChunkZ = CHUNK_NUM((int) m_world->getPlayer().getPosition().z);
    m_mainProgram->use();

    // set uniform values
    float lightPosition[] = {0, 1, 1, 1};
    float lightAmbient[] = {0.3f, 0.3f, 0.3f, 1};
    float lightDiffuse[] = {0.7f, 0.7f, 0.7f, 1};
    float lightSpecular[] = {1.0f, 1.0f, 1.0f, 1};

    m_mainProgram->setLight(lightPosition, lightAmbient, lightDiffuse, lightSpecular);

    m_blockModel->bind();
    for (int x = -drawDistance; x <= drawDistance; x++)
    {
        for (int z = -drawDistance; z <= drawDistance; z++)
        {
            Chunk* chunk = m_world->getChunk(x + playerChunkX, z + playerChunkZ);
            drawChunk(chunk);
        }
    }

    drawMobs();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    drawOverlays();
}

void Blocky::drawMobs()
{
    m_sphereTexture->bind();
    m_sphereModel->bind();

    for (SphereMob* mob : m_world->getMobs())
    {
        Matrix4 matrix;
        float offset = 0;//+0.5f;
        Vector pos = m_world->getPlayer().getPosition();
        double viewY = pos.y + m_world->getPlayer().getHeight() - 0.5;
        //matrix.rotateY(-m_world->getPlayer().getHeading());   // heading
//matrix.rotateX(m_world->getPlayer().getPitch());   // pitch
        matrix.rotateX(-90);
        matrix.rotateY(-mob->getHeading() - 90);
        matrix.translate(
            -pos.x + offset + mob->getPosition().x,
            ((float) mob->getPosition().y - viewY),
            -pos.z + offset + mob->getPosition().z);
        matrix.rotateY(m_world->getPlayer().getHeading());   // heading
        matrix.rotateX(m_world->getPlayer().getPitch());   // pitch

        Matrix4 matrixModelViewProjection = m_matrixProjection * matrix;
        Matrix4 matrixNormal;// = matrix;
        float c[] = {0, 0, 0, 1};
        matrixNormal.setColumn(3, c);

        Matrix4 modelView;
        modelView.rotateY(-m_world->getPlayer().getHeading());   // heading
        modelView.rotateX(-m_world->getPlayer().getPitch());   // pitch

        m_mainProgram->setMatrices(modelView, matrixModelViewProjection, matrixNormal, false);
        m_sphereModel->draw();
    }
}

void Blocky::drawChunk(Chunk* chunk)
{
    BlockType lastType = AIR;
    Vector pos = m_world->getPlayer().getPosition();
    double viewY = pos.y + m_world->getPlayer().getHeight();
    for (int chunkX = 0; chunkX < CHUNK_WIDTH; chunkX++)
    {
        int worldX = chunkX + chunk->getChunkX() * CHUNK_WIDTH;
        float x = (float) worldX - (float) pos.x;
        for (int chunkZ = 0; chunkZ < CHUNK_WIDTH; chunkZ++)
        {
            int worldZ = chunkZ + chunk->getChunkZ() * CHUNK_WIDTH;
            float z = (float) worldZ - (float) pos.z;
            for (int y = 0; y <= chunk->getMaxY(); y++)
            {
                const Block* block = chunk->getBlock(chunkX, y, chunkZ);

                if (block != nullptr && block->isVisible())
                {
                    BlockType type = block->getType();
                    bool highlight = false;
                    if (m_lookingAt == block || (worldX == floor(pos.x) && worldZ == floor(pos.z)))
                    {
                        //type = TARGET;
                        highlight = true;
                    }
                    if (type != lastType)
                    {
                        lastType = type;
                        switch (type)
                        {
                            case DIRT:
                                m_dirtTexture->bind();
                                break;
                            case GRASS:
                                m_grassTexture->bind();
                                break;
                            case STONE:
                                m_stoneTexture->bind();
                                break;
                            case TARGET:
                                m_targetTexture->bind();
                                break;
                            case AIR:
                            case EMPTY:
                                // How did we get here?
                                break;
                        }
                    }

                    Matrix4 matrix;
                    float offset = +0.5f;
                    matrix.translate(x + offset, ((float) y - viewY) + offset, z + offset);
                    //matrix *= m_matrixModelView;
                    //matrix *= rotateMatrix;
                    matrix.rotateY(m_world->getPlayer().getHeading());   // heading
                    matrix.rotateX(m_world->getPlayer().getPitch());   // pitch
                    drawBox(matrix, highlight);
                }
            }
        }
    }
}

bool Blocky::handleEvent(SDL_Event* event)
{
    Player& player = m_world->getPlayer();
    if (event->type == SDL_KEYDOWN)
    {
        if (event->key.keysym.sym == SDLK_w)
        {
            player.setForward(0.2f);
        }
        else if (event->key.keysym.sym == SDLK_s)
        {
            player.setForward(-0.2f);
        }
        else if (event->key.keysym.sym == SDLK_a)
        {
            player.setStrafe(-0.2f);
        }
        else if (event->key.keysym.sym == SDLK_d)
        {
            player.setStrafe(0.2f);
        }
        else if (player.getJump() <= 0 && event->key.keysym.sym == SDLK_SPACE)
        {
            player.setJump(1.0f);
        }
    }
    else if (event->type == SDL_KEYUP)
    {
        int sym = event->key.keysym.sym;
        if (sym >= SDLK_1 && sym <= SDLK_9)
        {
            player.setInventorySlot(sym - SDLK_1);
        }
        else if (sym == SDLK_0)
        {
            player.setInventorySlot(9);
        }
        else if (sym == SDLK_ESCAPE)
        {
            log(DEBUG, "ESCAPE!");
            m_moveMode = false;
            releaseMouse();

            m_menuOverlay->setVisible(true);
        }
    }
    else if (m_moveMode && event->type == SDL_MOUSEMOTION)
    {
        float heading = player.getHeading();
        float pitch = player.getPitch();
        heading += (float) (event->motion.xrel) * 0.1f;
        pitch += (float) (event->motion.yrel) * 0.1f;
        if (pitch > 90.0)
        {
            pitch = 90.0;
        }
        else if (pitch < -90.0)
        {
            pitch = -90.0;
        }
        player.setHeading(heading);
        player.setPitch(pitch);
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        if (m_moveMode && m_lookingAtMob != nullptr)
        {
            log(DEBUG, "You hit a mob! How rude!");
        }
        else if (m_moveMode && m_lookingAt != nullptr && m_lookingAtPos.y >= 0)
        {
            SDL_Keymod keyMod = SDL_GetModState();
            if (event->button.button == SDL_BUTTON_LEFT && (keyMod & KMOD_CTRL) == 0)
            {
                breakBlock();
            }
            else if ((event->button.button == SDL_BUTTON_LEFT && !!(keyMod & KMOD_CTRL)) ||
                event->button.button == SDL_BUTTON_RIGHT)
            {
                placeBlock();
            }
        }
        else if (!m_moveMode)
        {
            /*
            captureMouse();
            m_moveMode = true;
            //m_menuOverlay->setVisible(false);
             */
        }
    }
    else if (event->type == SDL_CONTROLLERAXISMOTION)
    {
        float pct = (float)event->caxis.value / (float)0x7fff;
        if (fabs(pct) < 0.5)
        {
            pct = 0;
        }

        log(DEBUG, "handleEvent: Controller Axis: axis=%d, value=%d, pct=%0.2f%%",
            event->caxis.axis,
            event->caxis.value,
            pct * 100.0f);

        if (event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
        {
            m_controllerForward = -pct;
        }
        if (event->caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
        {
            m_controllerStrafe = pct;
        }
        else if (event->caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX)
        {
            m_controllerHeading = pct;
        }
        else if (event->caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY)
        {
            m_controllerPitch = pct;
        }
    }
    else if (event->type == SDL_CONTROLLERBUTTONDOWN)
    {
        log(DEBUG, "handleEvent: Controller Button: type=%d, button=%d",
            event->cbutton.type,
            event->cbutton.button);
        if (event->cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER)
        {
            placeBlock();
        }
        else if (event->cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)
        {
            breakBlock();
            if (getGameController() != nullptr)
            {
                SDL_GameControllerRumble(
                    getGameController(),
                    0xFFFF * 3 / 4,
                    0xFFFF * 3 / 4,
                    250 );
            }
        }
        else if (player.getJump() <= 0 && event->cbutton.button == SDL_CONTROLLER_BUTTON_A)
        {
            player.setJump(1.0f);
        }
    }

    return true;
}

void Blocky::breakBlock()
{
    Player& player = m_world->getPlayer();
    Block* brokenBlock = m_world->getBlock(m_lookingAtPos);
    if (brokenBlock != nullptr)
    {
        log(DEBUG, "handleEvent: Broken block: type=%d", brokenBlock->getType());
        player.addBlockToInventory(brokenBlock->getType());
    }
    m_world->setBlock(m_lookingAtPos, nullptr, true);
    calcLookAt();
}

void Blocky::placeBlock()
{
    Player& player = m_world->getPlayer();

    Vector newPos = m_lookingAtPos;
    switch (m_lookingAtSide)
    {
        case X_NEG:
            newPos.x--;
            break;
        case X_POS:
            newPos.x++;
            break;
        case Y_NEG:
            newPos.y--;
            break;
        case Y_POS:
            newPos.y++;
            break;
        case Z_NEG:
            newPos.z--;
            break;
        case Z_POS:
            newPos.z++;
            break;
        default:
            log(DEBUG, "mainLoop: Place Block: invalid face: %d", m_lookingAtSide);
            return;
    }
    Block* block = m_world->getBlock(newPos);
    log(
        DEBUG, "mainLoop: Place Block: lookingAt=%d, %d, %d, side=%d, newPos=%d, %d, %d: block=%p",
        (int) m_lookingAtPos.x, (int) m_lookingAtPos.y, (int) m_lookingAtPos.z,
        m_lookingAtSide,
        (int) newPos.x, (int) newPos.y, (int) newPos.z,
        block);

    if (block == nullptr)
    {
        BlockContainer& container = player.getInventorySlot(
            player.getInventorySlot());
        if (container.type != EMPTY && container.count > 0)
        {
            m_world->setBlock(newPos, new Block(container.type), true);
            container.count--;
            calcLookAt();
        }
    }
}

void Blocky::calcLookAt()
{
    Player& player = m_world->getPlayer();

    // Which direction are we looking at?
    double xzLen = cos(player.getPitch() * (M_PI / 180.0));
    Vector v;
    v.x = xzLen * sin(-player.getHeading() * (M_PI / 180.0));
    v.y = sin(player.getPitch() * (M_PI / 180.0));
    v.z = xzLen * cos(player.getHeading() * (M_PI / 180.0));
    v = -v;

    Vector viewPos = player.getPosition();
    viewPos.y += player.getHeight();
    Ray ray(viewPos, v);

    m_lookingAt = nullptr;
    m_lookingAtSide = NONE;
    m_lookingAtMob = nullptr;
    Vector last;

    for (int i = 0; i < 25; i++)
    {
        float dist = (float)i * 0.2f;
        Vector rayPos = ray.at(dist);
        rayPos.x = floor(rayPos.x);
        rayPos.y = floor(rayPos.y);
        rayPos.z = floor(rayPos.z);

        if (rayPos.x == last.x && rayPos.y == last.y && rayPos.z == last.z)
        {
            // We've already checked this block!
            continue;
        }
        last = rayPos;

        if (m_lookingAtMob == nullptr)
        {
            for (SphereMob* mob: m_world->getMobs())
            {
                if (floor(mob->getPosition().x) == rayPos.x &&
                    floor(mob->getPosition().y) == rayPos.y &&
                    floor(mob->getPosition().z) == rayPos.z)
                {
                    log(DEBUG, "calcLookAt: Looking at a mob!");
                    m_lookingAtMob = mob;
                    break;
                }
            }
        }

        Block* block = m_world->getBlock(rayPos);
        if (block != nullptr && block->isVisible())
        {
            m_lookingAt = block;
            m_lookingAtPos = rayPos;
            break;
        }
    }
    if (m_lookingAt != nullptr)
    {
        m_lookingAtSide = m_blockModel->hit(m_lookingAtPos, &ray);
    }
}

void Blocky::update()
{
    if (fabs(m_controllerForward) > EPSILON)
    {
        m_world->getPlayer().setForward(0.2 * m_controllerForward);
    }
    if (fabs(m_controllerStrafe) > EPSILON)
    {
        m_world->getPlayer().setStrafe(0.2 * m_controllerStrafe);
    }
    if (fabs(m_controllerHeading) > EPSILON)
    {
        m_world->getPlayer().setHeading(m_world->getPlayer().getHeading() + (4.0 * m_controllerHeading));
    }
    if (fabs(m_controllerPitch) > EPSILON)
    {
        m_world->getPlayer().setPitch(m_world->getPlayer().getPitch() + (3.0 * m_controllerPitch));
    }

    bool changed = m_world->update();
    if (changed)
    {
        calcLookAt();
    }
}

void Blocky::exit()
{
    m_world->save();
}

BlockyApp::BlockyApp() : Frontier::FrontierApp(L"Blocky")
{
    setEngine(new WindowingEngine(this));
}
