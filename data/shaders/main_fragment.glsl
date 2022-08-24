#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

//#extension GL_OES_standard_derivatives : enable

// uniforms
layout (std140, binding=0) uniform bufferVals {
    mat4 lightPosition; // should be in the eye space
    vec4 lightAmbient;              // light ambient color
    vec4 lightDiffuse;              // light diffuse color
    vec4 lightSpecular;             // light specular color
    int highlight;
} lightInfo;
layout (binding=1) uniform sampler2D map0;                 // texture map 

layout (location=0) in vec3 esVertex;
layout (location=1) in vec3 esNormal;
layout (location=2) in vec2 texCoord0;

layout (location=3) out vec4 outColour;

float gridFactor (float parameter, float width, float feather) {
    float w1 = width - feather * 0.5;
    float d = fwidth(parameter);
    float looped = 0.5 - abs(mod(parameter, 1.0) - 0.5);
    return smoothstep(d * w1, d * (w1 + feather), looped);
}

float gridFactor (vec2 parameter, float width, float feather) {
    float w1 = width - feather * 0.5;
    vec2 d = fwidth(parameter);
    vec2 looped = 0.5 - abs(mod(parameter, 1.0) - 0.5);
    vec2 a2 = smoothstep(d * w1, d * (w1 + feather), looped);
    return min(a2.x, a2.y);
}

float gridFactor (vec3 parameter, float width, float feather) {
    float w1 = width - feather * 0.5;
    vec3 d = fwidth(parameter);
    vec3 looped = 0.5 - abs(mod(parameter, 1.0) - 0.5);
    vec3 a3 = smoothstep(d * w1, d * (w1 + feather), looped);
    return min(min(a3.x, a3.y), a3.z);
}

float gridFactor (vec4 parameter, float width, float feather) {
    float w1 = width - feather * 0.5;
    vec4 d = fwidth(parameter);
    vec4 looped = 0.5 - abs(mod(parameter, 1.0) - 0.5);
    vec4 a4 = smoothstep(d * w1, d * (w1 + feather), looped);
    return min(min(min(a4.x, a4.y), a4.z), a4.w);
}

float gridFactor (float parameter, float width) {
    float d = fwidth(parameter);
    float looped = 0.5 - abs(mod(parameter, 1.0) - 0.5);
    return smoothstep(d * (width - 0.5), d * (width + 0.5), looped);
}

float gridFactor (vec2 parameter, float width) {
    vec2 d = fwidth(parameter);
    vec2 looped = 0.5 - abs(mod(parameter, 1.0) - 0.5);
    vec2 a2 = smoothstep(d * (width - 0.5), d * (width + 0.5), looped);
    return min(a2.x, a2.y);
}

float gridFactor (vec3 parameter, float width) {
    vec3 d = fwidth(parameter);
    vec3 looped = 0.5 - abs(mod(parameter, 1.0) - 0.5);
    vec3 a3 = smoothstep(d * (width - 0.5), d * (width + 0.5), looped);
    return min(min(a3.x, a3.y), a3.z);
}

float gridFactor (vec4 parameter, float width) {
    vec4 d = fwidth(parameter);
    vec4 looped = 0.5 - abs(mod(parameter, 1.0) - 0.5);
    vec4 a4 = smoothstep(d * (width - 0.5), d * (width + 0.5), looped);
    return min(min(min(a4.x, a4.y), a4.z), a4.z);
}

/*
float gridFactor (vec2 vBC, float width, float feather) {
    float w1 = width - feather * 0.5;
    vec3 bary = vec3(vBC.x, vBC.y, 1.0 - vBC.x - vBC.y);
    vec3 d = fwidth(bary);
    vec3 a3 = smoothstep(d * w1, d * (w1 + feather), bary);
    return min(min(a3.x, a3.y), a3.z);
}

float gridFactor (vec2 vBC, float width) {
    vec3 bary = vec3(vBC.x, vBC.y, 1.0 - vBC.x - vBC.y);
    vec3 d = fwidth(bary);
    vec3 a3 = smoothstep(d * (width - 0.5), d * (width + 0.5), bary);
    return min(min(a3.x, a3.y), a3.z);
}
*/

void main()
{
    vec3 normal = normalize(esNormal);
    vec3 light;
    if (lightInfo.lightPosition.w == vec3(0.0, 0.0, 0.0))
    {
        light = normalize(lightInfo.lightPosition.xyz);
    }
    else
    {
        light = normalize(lightInfo.lightPosition.xyz - esVertex);
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

    // set frag color
    if (highlight == 0)
    {
        outColour = vec4(fragColor, 1.0);// keep opaque=1
    }
    else
    {
        outColour = mix(vec4(vec3(gridFactor(esVertex.xy * 5.0, 1.0)), 1), vec4(fragColor, 1.0), 0.75);
        //outColour = vec4(1.0, 0.0, 0.0, 1.0);
    }
    //outColour = mix(vec4(vec3(gridFactor(esVertex.xy * 5.0, 1.0)), 1), vec4(fragColor, 1.0), 0.5);
    //outColour = vec4(vec3(gridFactor(esVertex.xz, 1.0)), 1);
}
