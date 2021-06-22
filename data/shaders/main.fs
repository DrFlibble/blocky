// GLSL version
#version 110
// uniforms
uniform vec4 lightPosition;             // should be in the eye space
uniform vec4 lightAmbient;              // light ambient color
uniform vec4 lightDiffuse;              // light diffuse color
uniform vec4 lightSpecular;             // light specular color
uniform sampler2D map0;                 // texture map #1
// varyings
varying vec3 esVertex, esNormal;
varying vec3 color;
varying vec2 texCoord0;
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

    vec3 fragColor = lightAmbient.rgb * color;                  // begin with ambient
    float dotNL = max(dot(normal, light), 0.0);
    fragColor += lightDiffuse.rgb * color * dotNL;              // add diffuse
    fragColor *= texture2D(map0, texCoord0).rgb;                // modulate texture map
    float dotNH = max(dot(normal, halfv), 0.0);
    fragColor += pow(dotNH, 128.0) * lightSpecular.rgb * color; // add specular

    // set frag color
    gl_FragColor = vec4(fragColor, 1.0);  // keep opaque=1
}
