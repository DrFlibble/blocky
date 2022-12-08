#ifndef BRICK_PERLIN_H_
#define BRICK_PERLIN_H_

class PerlinNoise
{
 private:
    uint64_t m_seed;

    inline float noise1(int x, int y);
    inline float smooth1(int x, int y);
    inline float interpolate1(float x, float y);

 public:
    PerlinNoise();
    explicit PerlinNoise(uint64_t seed);
    float noise(float x, float y);
};

#endif
