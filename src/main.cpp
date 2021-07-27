
#include "texture.h"
#include "blockmodel.h"
#include "world.h"
#include "shader.h"
#include "blocky.h"
#include "ray.h"

using namespace std;
using namespace Geek;

Blocky::Blocky() : BlockyEngine("Blocky")
{
    m_player.set(0, 16.0, 0);
    m_world = new World();
}

Blocky::~Blocky() = default;

bool Blocky::initGame()
{
    m_blockModel = new BlockModel();
    bool res = m_blockModel->init();
    if (!res)
    {
        return false;
    }

    glGenVertexArrays(1, &m_skyVAO);

    m_dirtTexture = new Texture("../dirt-512.png");
    m_grassTexture = new Texture("../grass-512.png");
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

    // unbind GLSL
    GL(glUseProgram(0));

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

    // activate attribs
    GL(glEnableVertexAttribArray(0));
    GL(glEnableVertexAttribArray(1));
    GL(glEnableVertexAttribArray(2));

    GL(glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, m_blockModel->getPositionPtr()));
    GL(glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, m_blockModel->getNormalPtr()));
    GL(glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, m_blockModel->getTexCoordPtr()));

    GL(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr));

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);//attribVertexNormal);
    glDisableVertexAttribArray(2);//attribVertexTexCoord);

    //GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    //GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

}

void Blocky::drawFrame()
{
    GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

    /*
    Matrix4 matrixView;
    //matrixView.rotateY(m_heading);   // heading
    //matrixView.rotateX(m_pitch);   // pitch
     */


    Matrix4 matrixModel;

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

    GL(glUseProgram(0));

    SDL_GL_SwapWindow(m_window);
}


void Blocky::drawChunk(Chunk* chunk)
{
    //Matrix4 rotateMatrix;
    //rotateMatrix.rotateY(m_heading);   // heading
    //rotateMatrix.rotateX(m_pitch);   // pitch
    BlockType lastType = AIR;
    double viewY = m_player.y;
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
        if (m_lookingAt != nullptr && m_lookingAtPos.y > 0)
        {
            if (event->button.button == SDL_BUTTON_LEFT)
            {
                m_world->setBlock(m_lookingAtPos, nullptr, true);
                m_lookingAt = nullptr;
            }
            else // if (event->button.button == SDL_BUTTON_RIGHT)
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
                        return true;
                }
                Block* block = m_world->getBlock(newPos);
                log(
                    DEBUG, "mainLoop: lookingAt=%d, %d, %d, side=%d, newPos=%d, %d, %d: block=%p",
                    (int) m_lookingAtPos.x, (int) m_lookingAtPos.y, (int) m_lookingAtPos.z,
                    m_lookingAtSide,
                    (int) newPos.x, (int) newPos.y, (int) newPos.z,
                    block);
                if (block == nullptr)
                {
                    m_world->setBlock(newPos, new Block(GRASS), true);
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
    viewPos.y += 0.5f;
    Ray ray(viewPos, v);

    m_lookingAt = nullptr;
    m_lookingAtSide = NONE;
    Vector last;
    for (float dist = 0; dist < 10; dist += 0.1f)
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
        bool moving = fabs(m_forward) > EPSILON || fabs(m_strafe) > EPSILON;

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
        }
        m_lastMillis = time;

        bool canMove = true;
        int playerX = floor(checkPos.x);
        int playerY = floor(checkPos.y) - 1;
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
            }
        }
        if (canMove && !hasBlockUnderPlayer)
        {
            if (m_player.y > 0.20)
            {
                m_player.y -= 0.20;
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

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    Blocky blocky;

    bool res;
    res = blocky.init();
    if (!res)
    {
        return 0;
    }

    blocky.mainLoop();

    return 0;
}
