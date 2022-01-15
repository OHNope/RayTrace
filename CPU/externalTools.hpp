//
// Created by XBY on 2021/11/27.
//

#ifndef RAYTRACE_EXTERNALTOOLS_HPP
#define RAYTRACE_EXTERNALTOOLS_HPP

#include "./vec3.hpp"
#include <numbers>
#include <random>

using namespace std;

constexpr const auto M_TAU(2 * M_PI);
static inline float fast_cosc(float x) {
    x -= 0.25f + floorf(x + 0.25f);
    x *= 16.0f * (fabs(x) - 0.5f);
    x += 0.225f * x * (fabs(x) - 1.0f);
    return x;
}
inline float fast_sinf(float x) { return fast_cosc(x / M_TAU - 0.25f); }
inline float fast_cosf(float x) { return fast_cosc(x / M_TAU); }

constexpr inline int _BIT(int n) { return (1 << n); }
inline float fast_exp2(float x) {
    union {
        uint32_t i;
        float f;
    } v;
    float offset = (x < 0) ? 1.0f : 0.0f;
    float clipp = (x < -126) ? -126.0f : x;
    int w = clipp;
    float z = clipp - w + offset;
    v.i = (uint32_t)(_BIT(23) *
                     (clipp + 121.2740575f + 27.7280233f / (4.84252568f - z) -
                      1.49012907f * z));
    return v.f;
}
inline float fast_log2(float x) {
    union {
        float f;
        uint32_t i;
    } vx;
    union {
        uint32_t i;
        float f;
    } mx;
    vx.f = x;
    mx.i = (vx.i & 0x007FFFFF) | 0x3F000000;
    float y = vx.i;
    y *= 1.1920928955078125e-7f;
    return y - 124.22551499f - 1.498030302f * mx.f -
           1.72587999f / (0.3520887068f + mx.f);
}

constexpr inline long double operator"" _deg_to_rad(long double deg) {
    long double radians = deg * std::numbers::pi_v<long double> / 180;
    return radians;
}
constexpr inline long double operator"" _rad_to_deg(long double rad) {
    long double degrees = 180 * rad / std::numbers::pi_v<long double>;
    return degrees;
}

inline double InvSqrt(double number) {
    long long i;
    double x2, y;
    const double threehalfs = 1.5F;

    x2 = number * 0.5F;
    y = number;
    i = *(long long *)&y;
    i = 0x5fe6ec85e7de30da - (i >> 1);
    y = *(double *)&i;
    y = y * (threehalfs - (x2 * y * y)); // 1st iteration
    y = y * (threehalfs - (x2 * y * y)); // 2nd iteration, this can be removed
    return y;
}
inline float InvSqrt(float number) {
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y = number;
    i = *(long *)&y; // evil floating point bit level hacking
    i = 0x5f3759df - (i >> 1);
    y = *(float *)&i;
    y = y * (threehalfs - (x2 * y * y)); // 1st iteration
    y = y * (threehalfs - (x2 * y * y));
    return y;
}
#define FAST_SQRT(num) (1.0 / InvSqrt(num))

inline float acosFast4(float inX) {
    float x1 = abs(inX);
    float x2 = x1 * x1;
    float x3 = x2 * x1;
    float s;

    s = -0.2121144f * x1 + 1.5707288f;
    s = 0.0742610f * x2 + s;
    s = -0.0187293f * x3 + s;
    s = FAST_SQRT(1.0f - x1) * s;

    return inX >= 0.0f ? s : 3.1415926535897932384626433f - s;
}
inline float asinFast4(float inX) {
    float x = inX;
    // asin is offset of acos
    return 1.5707963267948966192313217f - acosFast4(x);
}
inline float atanFast4(float inX) {
    float x = inX;
    return x * (-0.1784f * abs(x) - 0.0663f * x * x + 1.0301f);
}

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
    double z = FAST_SQRT(1 - r2);
    double φ = 2 * M_PI * r1;
    double x = fast_cosf(φ) * 2 * FAST_SQRT(r2);
    double y = fast_sinf(φ) * 2 * FAST_SQRT(r2);
    return vec3(x, y, z);
}

#endif // RAYTRACE_EXTERNALTOOLS_HPP
