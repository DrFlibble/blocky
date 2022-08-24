#ifndef __BEYOND_PERLIN_H_
#define __BEYOND_PERLIN_H_

class PerlinNoise
{
 private:
    int m_seed;

    inline float noise1(int x, int y);
    inline float smooth1(int x, int y);
    inline float interpolate1(float x, float y);

 public:
    PerlinNoise();
    PerlinNoise(int seed);
    float noise(float x, float y);
};

#endif
