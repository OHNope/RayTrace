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

inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_double(min, max + 1));
}

inline double degrees_to_radians(double degrees) {
    return degrees * M_PI / 180.0;
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

inline vec3 random_cosine_direction() {
    double r1 = random_double();
    double r2 = random_double();
    double z = sqrt(1 - r2);
    double φ = 2 * M_PI * r1;
    double x = cos(φ) * 2 * sqrt(r2);
    double y = sin(φ) * 2 * sqrt(r2);
    return vec3(x, y, z);
}

#endif // RAYTRACE_EXTERNALTOOLS_HPP
