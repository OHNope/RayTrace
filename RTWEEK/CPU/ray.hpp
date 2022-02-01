//
// Created by XBY on 2021/11/27.
//

#ifndef RAYTRACE_RAY_HPP
#define RAYTRACE_RAY_HPP

#include "./vec3.hpp"

class ray {
public:
    ray() = default;
    ray(const vec3 &origin, const vec3 &direction, double time = 0.0)
        : A(origin), B(direction), tm(time) {}
    vec3 origin() const { return A; };
    vec3 direction() const { return B; };
    vec3 point_at(float t) const { return A + B * t; };
    double time() const { return tm; }

    vec3 A, B;
    double tm;
};

#endif // RAYTRACE_RAY_HPP
