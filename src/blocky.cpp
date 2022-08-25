
#include "texture.h"
#include "blockmodel.h"
#include "world.h"
#include "shader.h"
#include "blocky.h"
#include "ray.h"
#include "overlay.h"

#include <cwchar>

using namespace std;
using namespace Geek;
using namespace Geek::Gfx;

Blocky::Blocky() : BlockyEngine("Blocky")
{
    m_player.set(0, 16.0, 0);
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

    m_dirtTexture = new Texture("../dirt-512.png");
    m_grassTexture = new Texture("../grassblock.png");
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

    m_crossHairOverlay = new Overlay(this, 16, 16);
    m_infoOverlay = new Overlay(this, 200, 30);

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
    m_matrixModelView.identity();
    m_matrixModelView.rotateY(m_heading);   // heading
    m_matrixModelView.rotateX(m_pitch);   // pitch

    int drawDistance = 2;
    int playerChunkX = CHUNK_NUM((int) m_player.x);
    int playerChunkZ = CHUNK_NUM((int) m_player.z);

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

    Surface* crosshair = m_crossHairOverlay->getSurface();
    crosshair->clear(0xff000000);
    crosshair->drawRect(0, 7, 6, 2, 0xffffffff);
    crosshair->drawRect(9, 7, 6, 2, 0xffffffff);
    crosshair->drawRect(7, 0, 2, 6, 0xffffffff);
    crosshair->drawRect(7, 9, 2, 6, 0xffffffff);

    m_crossHairOverlay->draw((m_screenWidth / 2) - 8, (m_screenHeight / 2) - 8);

    Surface* infoSurface = m_infoOverlay->getSurface();
    infoSurface->clear(0xff000000);

    wchar_t buf[200];
    swprintf(buf, 200, L"X: %0.2f, Y: %0.2f, Z: %0.2f",
             m_player.x,
             m_player.y,
             m_player.z);
    m_fontManager->write(m_font, infoSurface, 0, 0, buf, 0xffffff, true, nullptr);

    if (m_lookingAt != nullptr)
    {
        swprintf(buf, 200, L"Looking at: X: %0.2f, Y: %0.2f, Z: %0.2f",
                 m_lookingAtPos.x,
                 m_lookingAtPos.y,
                 m_lookingAtPos.z);
        m_fontManager->write(m_font, infoSurface, 0, 12, buf, 0xffffff, true, nullptr);

    }

    m_infoOverlay->draw(0, 0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_BLEND);
}

void Blocky::drawChunk(Chunk* chunk)
{
    //Matrix4 rotateMatrix;
    //rotateMatrix.rotateY(m_heading);   // heading
    //rotateMatrix.rotateX(m_pitch);   // pitch
    BlockType lastType = AIR;
    double viewY = m_player.y + m_playerHeight;
    for (int chunkX = 0; chunkX < CHUNK_WIDTH; chunkX++)
    {
        int worldX = chunkX + chunk->getChunkX() * CHUNK_WIDTH;
        float x = (float) worldX - (float) m_player.x;
        for (int chunkZ = 0; chunkZ < CHUNK_WIDTH; chunkZ++)
        {
            int worldZ = chunkZ + chunk->getChunkZ() * CHUNK_WIDTH;
            float z = (float) worldZ - (float) m_player.z;
            for (int y = 0; y <= chunk->getMaxY(); y++)
            {
                const Block* block = chunk->getBlock(chunkX, y, chunkZ);

                if (block != nullptr && block->isVisible())
                {
                    BlockType type = block->getType();
                    bool highlight = false;
                    if (m_lookingAt == block || worldX == floor(m_player.x) && worldZ == floor(m_player.z))
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
                            case TARGET:
                                m_targetTexture->bind();
                                break;
                            case AIR:
                                // How did we get here?
                                break;
                        }
                    }

                    Matrix4 matrix;
                    float offset = +0.5f;
                    matrix.translate(x + offset, ((float) y - viewY) + offset, z + offset);
                    //matrix *= m_matrixModelView;
                    //matrix *= rotateMatrix;
                    matrix.rotateY(m_heading);   // heading
                    matrix.rotateX(m_pitch);   // pitch
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
            m_forward = 0.2f;
        }
        else if (event->key.keysym.sym == SDLK_s)
        {
            m_forward = -0.2f;
        }
        else if (event->key.keysym.sym == SDLK_a)
        {
            m_strafe = -0.2f;
        }
        else if (event->key.keysym.sym == SDLK_d)
        {
            m_strafe = 0.2f;
        }
        else if (m_jump <= 0 && event->key.keysym.sym == SDLK_SPACE)
        {
            m_jump = 1.0f;
        }
    }
    else if (event->type == SDL_MOUSEMOTION)
    {
        m_heading += (float) (event->motion.x - 400) * 0.1f;
        m_pitch += (float) (event->motion.y - 300) * 0.1f;
        if (m_pitch > 90.0)
        {
            m_pitch = 90.0;
        }
        else if (m_pitch < -90.0)
        {
            m_pitch = -90.0;
        }
        SDL_WarpMouseInWindow(m_window, 400, 300);
    }
    else if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        if (m_lookingAt != nullptr && m_lookingAtPos.y >= 0)
        {
            SDL_Keymod keyMod = SDL_GetModState();
            if (event->button.button == SDL_BUTTON_LEFT && (keyMod & KMOD_CTRL) == 0)
            {
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
                    m_world->setBlock(newPos, new Block(GRASS), true);
                    calcLookAt();
                }
            }
        }

    }
    return true;
}

void Blocky::calcLookAt()
{
    // Which direction are we looking at?
    float xzLen = cosf((float) m_pitch * ((float)M_PI / 180.0f));
    Vector v;
    v.x = xzLen * sinf(-m_heading * ((float)M_PI / 180.0f));
    v.y = sinf(m_pitch * ((float)M_PI / 180.0f));
    v.z = xzLen * cosf(m_heading * ((float)M_PI / 180.0f));
    v = -v;

    Vector viewPos = m_player;
    viewPos.y += m_playerHeight;
    Ray ray(viewPos, v);

    m_lookingAt = nullptr;
    m_lookingAtSide = NONE;
    Vector last;
    for (float dist = 0; dist < 10.0; dist += 0.1f)
    {
        Vector v = ray.at(dist);
        v.x = floor(v.x);
        v.y = floor(v.y);
        v.z = floor(v.z);

        if (v.x == last.x && v.y == last.y && v.z == last.z)
        {
            // We've already checked this block!
            continue;
        }
        last = v;

        Block* block = m_world->getBlock(v);
        if (block != nullptr && block->isVisible())
        {
            m_lookingAt = block;
            m_lookingAtPos = v;
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
    double headingSin = sin(m_heading * (M_PI / 180.0));
    double headingCos = cos(m_heading * (M_PI / 180.0));

    uint64_t time = SDL_GetTicks();
    if ((time - m_lastMillis) > (1000 / 60))
    {
        Vector origPosition = m_player;
        bool moving = fabs(m_forward) > EPSILON || fabs(m_strafe) > EPSILON || m_jump > 0;

        Vector checkPos = m_player;
        if (moving)
        {
            m_player.x +=
                headingSin * m_forward
                +
                headingCos * m_strafe;
            m_player.z += headingCos * -m_forward +
                          headingSin * m_strafe;
            checkPos.x +=
                headingSin * (m_forward
                              * 2) +
                headingCos * (m_strafe
                              * 2);
            checkPos.z += headingCos * -(m_forward * 2) +
                          headingSin * (m_strafe
                                        * 2);
            m_forward *= 0.75f;
            m_strafe *= 0.5f;

            if (m_jump > 0)
            {
                if (m_jump > 0.5)
                {
                    m_player.y += 0.2;
                }
                else
                {
                    m_player.y -= 0.2;
                }
                m_jump -= 0.1;
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
                m_player.y++;
                hasBlockUnderPlayer = true;
            }
        }

        if (canMove && !hasBlockUnderPlayer && m_jump <= 0)
        {
            // Falling!
            if (m_player.y > 0)
            {
                m_player.y -= 1;
            }
        }

        if (!canMove)
        {
            // Nope! No moving for you!
            m_player = origPosition;
            m_forward = 0.0f;
            m_strafe = 0.0f;
        }

        calcLookAt();
    }
}
