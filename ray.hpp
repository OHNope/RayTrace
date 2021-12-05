//
// Created by XBY on 2021/11/27.
//

#ifndef RAYTRACE_RAY_HPP
#define RAYTRACE_RAY_HPP

#include "./vec3.hpp"

class ray {
public:
    ray() = default;
    ray(const vec3 &a, const vec3 &b) : A(a), B(b){};
    vec3 origin() const { return A; };
    vec3 direction() const { return B; };
    vec3 point_at(float t) const { return A + B * t; };

    vec3 A, B;
};

#endif // RAYTRACE_RAY_HPP
