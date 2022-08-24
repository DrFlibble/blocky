#version 400

uniform sampler2D map0;                 // texture map
in vec2 texCoord;
out vec4 outputColor;

void main()
{
	outputColor = texture(map0, texCoord);
}
