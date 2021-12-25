//
// Created by XBY on 2021/11/27.
//

#ifndef RAYTRACE_VEC3_HPP
#define RAYTRACE_VEC3_HPP

#include <cmath>
#include <cstdlib>

inline double random_double(double min, double max);

class vec3 {
public:
    vec3() : e{0, 0, 0} {}
    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double &operator[](int i) { return e[i]; }

    vec3 &operator+=(const vec3 &v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3 &operator*=(const double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3 &operator/=(const double t) { return *this *= 1 / t; }

    bool operator<(const vec3 &x) const {
        return this->length_squared() < x.length_squared();
    }

    double length() const { return sqrt(length_squared()); }

    double length_squared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }
    bool near_zero() const {
        // Return true if the vector is close to zero in all dimensions.
        const auto s = 1e-8;
        return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
    }
    inline static vec3 random() {
        return vec3(random_double(1, 0), random_double(1, 0),
                    random_double(1, 0));
    }

    inline static vec3 random(double min, double max) {
        return vec3(random_double(min, max), random_double(min, max),
                    random_double(min, max));
    }

public:
    double e[3];
};

inline vec3 operator+(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3 &v) {
    return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3 &v, double t) { return t * v; }

inline vec3 operator/(vec3 v, double t) { return (1 / t) * v; }
inline vec3 operator/(vec3 v, vec3 t) {
    return vec3(v[0] / t[0], v[1] / t[1], v[2] / t[2]);
}

inline double dot(const vec3 &u, const vec3 &v) {
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v) { return v / v.length(); }

vec3 random_in_unit_disk() {
    while (true) {
        vec3 p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_squared() >= 1)
            continue;
        return p;
    }
}

vec3 random_in_unit_sphere() {
    while (true) {
        auto p = vec3::random(-1, 1);
        if (p.length_squared() >= 1)
            continue;
        return p;
    }
};

inline vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}

using point3 = vec3;
using color = vec3;

#endif // RAYTRACE_VEC3_HPP
