
#include "texture.h"
#include "blockmodel.h"
#include "world.h"
#include "shader.h"
#include "blocky.h"
#include "ray.h"

using namespace std;
using namespace Geek;
using namespace Geek::Gfx;

Blocky::Blocky() : BlockyEngine("Blocky")
{
    m_world = new World();
}

Blocky::~Blocky() = default;

bool Blocky::initGame()
{
    glGenVertexArrays(1, &m_overlayVAO);
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

    // set uniform values
    float lightPosition[] = {0, 0, 1, 1};
    float lightAmbient[] = {0.3f, 0.3f, 0.3f, 1};
    float lightDiffuse[] = {0.7f, 0.7f, 0.7f, 1};
    float lightSpecular[] = {1.0f, 1.0f, 1.0f, 1};

    m_mainProgram->setLight(lightPosition, lightAmbient, lightDiffuse, lightSpecular);

    m_skyProgram = new SkyShader();
    res = m_skyProgram->load();
    if (!res)
    {
        return false;
    }

    m_skyProgram->unuse();

    m_overlayProgram = new OverlayShader();
    res = m_overlayProgram->load();
    if (!res)
    {
        return false;
    }

    m_overlayProgram->unuse();

    m_crossHairOverlay = new CrossHairOverlay(this);
    m_infoOverlay = new InfoOverlay(this);
    m_statusBarOverlay = new StatusBarOverlay(this);

    return true;
}

void Blocky::drawBox(const Matrix4 &matrix, bool highlight)
{
    // set matrix uniforms every drawFrame
    Matrix4 matrixModelViewProjection = m_matrixProjection * matrix;
    Matrix4 matrixNormal = matrix;
    float c[] = {0, 0, 0, 1};
    matrixNormal.setColumn(3, c);

    m_mainProgram->setMatrices(matrix, matrixModelViewProjection, matrixNormal, highlight);

    m_blockModel->draw();
}

void Blocky::drawFrame()
{
    m_matrixModelView = m_world->getPlayer().getMatrix();

    int drawDistance = 2;
    int playerChunkX = CHUNK_NUM((int) m_world->getPlayer().getPosition().x);
    int playerChunkZ = CHUNK_NUM((int) m_world->getPlayer().getPosition().z);

    // Sky
    m_skyProgram->use();
    float time = (float) SDL_GetTicks() / 500.0f;
    m_skyProgram->setTime(time);
    m_skyProgram->setMatrices(m_matrixModelView, m_matrixProjection);
    glBindVertexArray(m_skyVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_mainProgram->use();
    m_blockModel->bind();
    for (int x = -drawDistance; x <= drawDistance; x++)
    {
        for (int z = -drawDistance; z <= drawDistance; z++)
        {
            Chunk* chunk = m_world->getChunk(x + playerChunkX, z + playerChunkZ);
            drawChunk(chunk);
        }
    }

    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    m_crossHairOverlay->draw();
    m_infoOverlay->draw();
    m_statusBarOverlay->draw();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_BLEND);
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
    if (event->type == SDL_KEYDOWN)
    {
        if (event->key.keysym.sym == SDLK_w)
        {
            m_world->getPlayer().setForward(0.2f);
        }
        else if (event->key.keysym.sym == SDLK_s)
        {
            m_world->getPlayer().setForward(-0.2f);
        }
        else if (event->key.keysym.sym == SDLK_a)
        {
            m_world->getPlayer().setStrafe(-0.2f);
        }
        else if (event->key.keysym.sym == SDLK_d)
        {
            m_world->getPlayer().setStrafe(0.2f);
        }
        else if (m_world->getPlayer().getJump() <= 0 && event->key.keysym.sym == SDLK_SPACE)
        {
            m_world->getPlayer().setJump(1.0f);
        }
    }
    else if (event->type == SDL_KEYUP)
    {
        int sym = event->key.keysym.sym;
        if (sym >= SDLK_1 && sym <= SDLK_9)
        {
            m_world->getPlayer().setInventorySlot(sym - SDLK_1);
        }
        else if (sym == SDLK_0)
        {
            m_world->getPlayer().setInventorySlot(9);
        }
        else if (sym == SDLK_ESCAPE)
        {
            log(DEBUG, "ESCAPE!");
            m_moveMode = false;
            releaseMouse();
        }
    }
    else if (m_moveMode && event->type == SDL_MOUSEMOTION)
    {
        float heading = m_world->getPlayer().getHeading();
        float pitch = m_world->getPlayer().getPitch();
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
        m_world->getPlayer().setHeading(heading);
        m_world->getPlayer().setPitch(pitch);
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        if (m_moveMode && m_lookingAt != nullptr && m_lookingAtPos.y >= 0)
        {
            SDL_Keymod keyMod = SDL_GetModState();
            if (event->button.button == SDL_BUTTON_LEFT && (keyMod & KMOD_CTRL) == 0)
            {
                Block* brokenBlock = m_world->getBlock(m_lookingAtPos);
                if (brokenBlock != nullptr)
                {
                    log(DEBUG, "handleEvent: Broken block: type=%d", brokenBlock->getType());
                    m_world->getPlayer().addBlockToInventory(brokenBlock->getType());
                }
                m_world->setBlock(m_lookingAtPos, nullptr, true);
                calcLookAt();
            }
            else if ((event->button.button == SDL_BUTTON_LEFT && !!(keyMod & KMOD_CTRL)) ||
                event->button.button == SDL_BUTTON_RIGHT)
            {
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
                        return true;
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
                    BlockContainer& container = m_world->getPlayer().getInventorySlot(
                    m_world->getPlayer().getInventorySlot());
                    if (container.type != EMPTY && container.count > 0)
                    {
                        m_world->setBlock(newPos, new Block(container.type), true);
                        container.count--;
                        calcLookAt();
                    }
                }
            }
        }
        else if (!m_moveMode)
        {
            captureMouse();
            m_moveMode = true;
        }
    }
    return true;
}

void Blocky::calcLookAt()
{
    // Which direction are we looking at?
    double xzLen = cos(m_world->getPlayer().getPitch() * (M_PI / 180.0));
    Vector v;
    v.x = xzLen * sin(-m_world->getPlayer().getHeading() * (M_PI / 180.0));
    v.y = sin(m_world->getPlayer().getPitch() * (M_PI / 180.0));
    v.z = xzLen * cos(m_world->getPlayer().getHeading() * (M_PI / 180.0));
    v = -v;

    Vector viewPos = m_world->getPlayer().getPosition();
    viewPos.y += m_world->getPlayer().getHeight();
    Ray ray(viewPos, v);

    m_lookingAt = nullptr;
    m_lookingAtSide = NONE;
    Vector last;
    for (float dist = 0; dist < 10.0; dist += 0.2f)
    {
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
    double headingSin = sin(m_world->getPlayer().getHeading() * (M_PI / 180.0));
    double headingCos = cos(m_world->getPlayer().getHeading() * (M_PI / 180.0));

    uint64_t time = SDL_GetTicks();
    if ((time - m_lastMillis) > (1000 / 60))
    {
        Vector origPosition = m_world->getPlayer().getPosition();
        bool moving =
            fabs(m_world->getPlayer().getForward()) > EPSILON ||
            fabs(m_world->getPlayer().getStrafe()) > EPSILON ||
            m_world->getPlayer().getJump() > 0;

        Vector checkPos = m_world->getPlayer().getPosition();
        if (moving)
        {
            m_world->getPlayer().getPosition().x +=
                headingSin * m_world->getPlayer().getForward()
                +
                headingCos * m_world->getPlayer().getStrafe();
            m_world->getPlayer().getPosition().z += headingCos * -m_world->getPlayer().getForward() +
                          headingSin * m_world->getPlayer().getStrafe();
            checkPos.x +=
                headingSin * (m_world->getPlayer().getForward()
                              * 2) +
                headingCos * (m_world->getPlayer().getStrafe()
                              * 2);
            checkPos.z += headingCos * -(m_world->getPlayer().getForward() * 2) +
                          headingSin * (m_world->getPlayer().getStrafe()
                                        * 2);
            m_world->getPlayer().setForward(m_world->getPlayer().getForward() * 0.75f);
            m_world->getPlayer().setStrafe(m_world->getPlayer().getStrafe() * 0.75f);

            if (m_world->getPlayer().getJump() > 0)
            {
                if (m_world->getPlayer().getJump() > 0.5)
                {
                    m_world->getPlayer().getPosition().y += 0.2;
                }
                else
                {
                    m_world->getPlayer().getPosition().y -= 0.2;
                }
                m_world->getPlayer().setJump(m_world->getPlayer().getJump() - 0.1);
            }
        }
        m_lastMillis = time;

        bool canMove = true;
        int playerX = floor(checkPos.x);
        int playerY = floor(checkPos.y);
        int playerZ = floor(checkPos.z);
        Block* nextBlockUnderPlayer = m_world->getBlock(
            playerX,
            playerY - 1,
            playerZ);
        bool hasBlockUnderPlayer = nextBlockUnderPlayer != nullptr;
        if (moving)
        {
            Block* nextBlockAtPlayer = m_world->getBlock(playerX, playerY, playerZ);
            Block* nextBlockAbovePlayer = m_world->getBlock(
                playerX,
                playerY + 1,
                playerZ);

            bool hasBlockAtPlayer = nextBlockAtPlayer != nullptr;
            bool hasBlockAbovePlayer = nextBlockAbovePlayer != nullptr;

            if (hasBlockAbovePlayer)
            {
                canMove = false;
            }
            else if (hasBlockUnderPlayer && !hasBlockAtPlayer)
            {
                canMove = true;
            }
            else if (hasBlockAtPlayer)
            {
                canMove = true;
                m_world->getPlayer().getPosition().y++;
                hasBlockUnderPlayer = true;
            }
        }

        if (canMove && !hasBlockUnderPlayer && m_world->getPlayer().getJump() <= 0)
        {
            // Falling!
            if (m_world->getPlayer().getPosition().y > 0)
            {
                m_world->getPlayer().getPosition().y -= 1;
            }
        }

        if (!canMove)
        {
            // Nope! No moving for you!
            m_world->getPlayer().setPosition(origPosition);
            m_world->getPlayer().setForward(0.0f);
            m_world->getPlayer().setStrafe(0.0f);
        }

        calcLookAt();
    }
}

void Blocky::exit()
{
    m_world->save();
}
