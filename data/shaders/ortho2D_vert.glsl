#version 400

uniform float positionX;
uniform float positionY;
uniform float width;
uniform float height;
uniform float textureWidth;
uniform float textureHeight;

out vec2 texCoord;

const vec2 data[4] = vec2[]
(
    vec2(-1.0,  1.0),
    vec2(-1.0, -1.0),
    vec2( 1.0,  1.0),
    vec2( 1.0, -1.0)
);

const vec2 texCoordData[4] = vec2[]
(
    vec2(0.0,  0.0),
    vec2(0.0, 1.0),
    vec2(1.0,  0.0),
    vec2(1.0, 1.0)
);

void main()
{
    float x0 = (((positionX) * 2) - 1);
    float y0 = (((1 - positionY) * 2) - 1);
    float x1 = x0 + (width * 2);
    float y1 = y0 - (height * 2);

    switch (gl_VertexID)
    {
        case 0:
            gl_Position = vec4(x0, y0, 1.0, 1.0);
            texCoord = texCoordData[0];
            break;

        case 1:
            gl_Position = vec4(x0, y1, 1.0, 1.0);
            texCoord = texCoordData[1];
            break;

        case 2:
            gl_Position = vec4(x1, y0, 1.0, 1.0);
            texCoord = texCoordData[2];
            break;

        case 3:
            gl_Position = vec4(x1, y1, 1.0, 1.0);
            texCoord = texCoordData[3];
            break;
    }
    texCoord.x *= textureWidth;
    texCoord.y *= textureHeight;
}
