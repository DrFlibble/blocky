//
//

#include "brick/ray.h"

bool AABB::hit(const Ray& r, double t_min, double t_max) const
{
    for (int a = 0; a < 3; a++)
    {
        auto invD = 1.0f / r.direction().f[a];
        auto t0 = (min().f[a] - r.origin().f[a]) * invD;
        auto t1 = (max().f[a] - r.origin().f[a]) * invD;
        if (invD < 0.0f)
        {
            std::swap(t0, t1);
        }
        t_min = t0 > t_min ? t0 : t_min;
        t_max = t1 < t_max ? t1 : t_max;
        if (t_max <= t_min)
        {
            return false;
        }
    }
    return true;
}
