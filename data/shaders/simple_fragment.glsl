// GLSL version
#version 330

//#extension GL_OES_standard_derivatives : enable

// uniforms
uniform vec4 lightPosition;             // should be in the eye space
uniform vec4 lightAmbient;              // light ambient color
uniform vec4 lightDiffuse;              // light diffuse color
uniform vec4 lightSpecular;             // light specular color
uniform sampler2D map0;                 // texture map #1
uniform int highlight;

in vec3 esVertex;
in vec3 esNormal;
in vec2 texCoord0;

out vec4 outColour;

void main()
{
    vec3 normal = normalize(esNormal);
    vec3 light;
    if(lightPosition.w == 0.0)
    {
        light = normalize(lightPosition.xyz);
    }
    else
    {
        light = normalize(lightPosition.xyz - esVertex);
    }
    vec3 view = normalize(-esVertex);
    vec3 halfv = normalize(light + view);

    vec3 color = vec3(1.0, 1.0, 1.0);
    vec3 fragColor = lightAmbient.rgb * color;                  // begin with ambient
    float dotNL = max(dot(normal, light), 0.0);
    fragColor += lightDiffuse.rgb * color * dotNL;              // add diffuse
    fragColor *= texture(map0, texCoord0).rgb;                // modulate texture map
    float dotNH = max(dot(normal, halfv), 0.0);
    fragColor += pow(dotNH, 128.0) * lightSpecular.rgb * color; // add specular

    //outColour = vec4(fragColor, 1.0);// keep opaque=1
    //outColour = vec4(1.0, 1.0, 1.0, 1.0);
    outColour = vec4(texture(map0, texCoord0).rgb, 1.0);                // modulate texture map
}
