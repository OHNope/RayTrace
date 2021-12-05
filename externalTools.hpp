//
// Created by XBY on 2021/11/27.
//

#ifndef RAYTRACE_EXTERNALTOOLS_HPP
#define RAYTRACE_EXTERNALTOOLS_HPP

#include "./vec3.hpp"
#include <random>
using namespace std;

static double get_random() { //[0,1)
    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(mt);
}

vec3 random_in_unit_sphere() {
    vec3 p;
    do
        p = 2.0 * vec3(get_random(), get_random(), get_random()) -
            vec3(1, 1, 1);
    while (p.length_squared() >= 1.0);
    return p;
};

inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

inline double degrees_to_radians(double degrees) {
    return degrees * M_PI / 180.0;
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

#endif // RAYTRACE_EXTERNALTOOLS_HPP
