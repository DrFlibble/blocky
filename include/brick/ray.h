#ifndef BLOCKY_RAY_H
#define BLOCKY_RAY_H

#include <geek/core-maths.h>

class Ray
{
 private:
    Geek::Vector m_orig;
    Geek::Vector m_dir;

 public:
    Ray() = default;
    Ray(const Geek::Vector& orig, const Geek::Vector& dir) : m_orig(orig), m_dir(dir) {}

    [[nodiscard]] Geek::Vector at(double t) const
    {
        return m_orig + (m_dir * t);
    }

    [[nodiscard]] Geek::Vector origin() const
    {
        return m_orig;
    }

    [[nodiscard]] Geek::Vector direction() const
    {
        return m_dir;
    }
};

class AABB
{
 private:
    Geek::Vector m_minimum;
    Geek::Vector m_maximum;

 public:
    [[nodiscard]] Geek::Vector min() const {return m_minimum; }
    [[nodiscard]] Geek::Vector max() const {return m_maximum; }

    [[nodiscard]] bool hit(const Ray& r, double t_min, double t_max) const;
};

#endif //BLOCKY_RAY_H
