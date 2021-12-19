//
// Created by XBY on 2021/12/19.
//

#ifndef RAYTRACE_AABB_HPP
#define RAYTRACE_AABB_HPP

#include "./ray.hpp"
#include "./vec3.hpp"

class AABB {
public:
    AABB() = default;
    AABB(const vec3 &a, const vec3 &b) {
        _min = a;
        _max = b;
    }

    vec3 min() const { return _min; }
    vec3 max() const { return _max; }

    bool hit(const ray &r, double tmin, double tmax) const;
    vec3 _min;
    vec3 _max;
};
inline bool AABB::hit(const ray &r, double tmin, double tmax) const {
    for (int a = 0; a < 3; a++) {
        auto invD = 1.0f / r.direction()[a];
        auto t0 = (min()[a] - r.origin()[a]) * invD;
        auto t1 = (max()[a] - r.origin()[a]) * invD;
        if (invD < 0.0f)
            std::swap(t0, t1);
        tmin = t0 > tmin ? t0 : tmin;
        tmax = t1 < tmax ? t1 : tmax;
        if (tmax <= tmin)
            return false;
    }
    return true;
}

AABB surrounding_box(AABB box0, AABB box1) {
    vec3 small(fmin(box0.min().x(), box1.min().x()),
               fmin(box0.min().y(), box1.min().y()),
               fmin(box0.min().z(), box1.min().z()));
    vec3 big(fmax(box0.max().x(), box1.max().x()),
             fmax(box0.max().y(), box1.max().y()),
             fmax(box0.max().z(), box1.max().z()));
    return AABB(small, big);
}

#endif // RAYTRACE_AABB_HPP
