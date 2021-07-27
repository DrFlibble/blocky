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
