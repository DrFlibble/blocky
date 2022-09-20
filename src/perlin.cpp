/* A simple Perlin noise generator */

#include <cmath>
#include <cstdlib>

#include "perlin.h"

PerlinNoise::PerlinNoise()
{
    m_seed = (uint64_t)rand() << 32;
    m_seed |= (uint64_t)rand();
}

PerlinNoise::PerlinNoise(uint64_t seed)
{
    m_seed = seed;
}

float PerlinNoise::noise1(int x, int y)
{
    uint64_t r1;
    uint64_t n;

    n = x + y * 57;
    n *= m_seed;
    n = (n<<13) ^ n;
    r1 = ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff);

    return 1.0f - (float)r1 / 1073741824.0f;
}

float PerlinNoise::smooth1(int x, int y)
{
    float a;
    float b;
    float c;

    a = (noise1(x - 1, y - 1) +
	 noise1(x + 1, y - 1) +
	 noise1(x - 1, y + 1) +
	 noise1(x + 1, y + 1)) / 16;

    b = (noise1(x - 1, y) +
	 noise1(x + 1, y) +
	 noise1(x, y - 1) +
	 noise1(x, y + 1)) / 8;

    c =  noise1(x, y) / 4;

    return a + b + c;
}

static float perlin_interpolate_cosine(float a, float b, float x)
{
    float ft = x * (float)M_PI;
    float f = (1.0f - cos(ft)) * 0.5f;

    return (a * (1.0f - f)) + (b * f);
}

float PerlinNoise::interpolate1(float x, float y)
{
    int ix = int(x);
    int iy = int(y);
    float fx = x - floorf(x);
    float fy = y - floorf(y);

    float v1 = smooth1(ix    , iy);
    float v2 = smooth1(ix + 1, iy);
    float v3 = smooth1(ix    , iy + 1);
    float v4 = smooth1(ix + 1, iy + 1);

    float i1 = perlin_interpolate_cosine(v1, v2, fx);
    float i2 = perlin_interpolate_cosine(v3, v4, fx);

    return perlin_interpolate_cosine(i1, i2, fy);
}

float PerlinNoise::noise(float x, float y)
{
    float total = 0.0f;
    float p = 0.02f; // Persistence
    int n = 2;

    x *= 0.5f;
    y *= 0.5f;

    for (int i = 0; i < n; i++)
    {
	    float freq = powf(2.0f, (float)i);
	    float amp = powf(p, (float)i);
        total += interpolate1(x * freq, y * freq) * amp;
    }
    return total;
}


