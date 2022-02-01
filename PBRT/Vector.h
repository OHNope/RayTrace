//
// Created by XBY on 2022/1/29.
//

#ifndef RAYTRACE_Vector_H
#define RAYTRACE_Vector_H

#include <cmath>

template <typename T> class Vector2 {
public:
    Vector2() { x = y = 0; }
    Vector2(T xx, T yy) : x(xx), y(yy) { Assert(!HasNaNs()); }
    bool HasNaNs() const { return std::isnan(x) || std::isnan(y); }
    explicit Vector2(const Point2<T> &p);
    explicit Vector2(const Point3<T> &p);
#ifndef NDEBUG
    // The default versions of these are fine for release builds; for debug
    // we define them so that we can add the Assert checks.
    Vector2(const Vector2<T> &v) {
        Assert(!v.HasNaNs());
        x = v.x;
        y = v.y;
    }
    Vector2<T> &operator=(const Vector2<T> &v) {
        Assert(!v.HasNaNs());
        x = v.x;
        y = v.y;
        return *this;
    }
#endif // !NDEBUG
    friend std::ostream &operator<<(std::ostream &os, const Vector2<T> &v) {
        os << "[" << v.x << ", " << v.y << "]";
        return os;
    }

    Vector2<T> operator+(const Vector2<T> &v) const {
        Assert(!v.HasNaNs());
        return Vector2(x + v.x, y + v.y);
    }

    Vector2<T> &operator+=(const Vector2<T> &v) {
        Assert(!v.HasNaNs());
        x += v.x;
        y += v.y;
        return *this;
    }
    Vector2<T> operator-(const Vector2<T> &v) const {
        Assert(!v.HasNaNs());
        return Vector2(x - v.x, y - v.y);
    }

    Vector2<T> &operator-=(const Vector2<T> &v) {
        Assert(!v.HasNaNs());
        x -= v.x;
        y -= v.y;
        return *this;
    }
    bool operator==(const Vector2<T> &v) const { return x == v.x && y == v.y; }
    bool operator!=(const Vector2<T> &v) const { return x != v.x || y != v.y; }
    Vector2<T> operator*(T f) const { return Vector2<T>(f * x, f * y); }

    Vector2<T> &operator*=(T f) {
        Assert(!std::isnan(f));
        x *= f;
        y *= f;
        return *this;
    }
    Vector2<T> operator/(T f) const {
        Assert(f != 0);
        Float inv = (Float)1 / f;
        return Vector2<T>(x * inv, y * inv);
    }

    Vector2<T> &operator/=(T f) {
        Assert(f != 0);
        Float inv = (Float)1 / f;
        x *= inv;
        y *= inv;
        return *this;
    }
    Vector2<T> operator-() const { return Vector2<T>(-x, -y); }
    T operator[](int i) const {
        Assert(i >= 0 && i <= 1);
        if (i == 0)
            return x;
        return y;
    }

    T &operator[](int i) {
        Assert(i >= 0 && i <= 1);
        if (i == 0)
            return x;
        return y;
    }
    Float LengthSquared() const { return x * x + y * y; }
    Float Length() const { return std::sqrt(LengthSquared()); }

public:
    T x, y;
} template <typename T> class Vector3 {
public:
    T operator[](int i) const {
        Assert(i >= 0 && i <= 2);
        if (i == 0)
            return x;
        if (i == 1)
            return y;
        return z;
    }
    T &operator[](int i) {
        Assert(i >= 0 && i <= 2);
        if (i == 0)
            return x;
        if (i == 1)
            return y;
        return z;
    }
    Vector3() { x = y = z = 0; }
    Vector3(T x, T y, T z) : x(x), y(y), z(z) { Assert(!HasNaNs()); }
    bool HasNaNs() const {
        return std::isnan(x) || std::isnan(y) || std::isnan(z);
    }
    explicit Vector3(const Point3<T> &p);
#ifndef NDEBUG
    // The default versions of these are fine for release builds; for debug
    // we define them so that we can add the Assert checks.
    Vector3(const Vector3<T> &v) {
        Assert(!v.HasNaNs());
        x = v.x;
        y = v.y;
        z = v.z;
    }

    Vector3<T> &operator=(const Vector3<T> &v) {
        Assert(!v.HasNaNs());
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
#endif // !NDEBUG
    friend std::ostream &operator<<(std::ostream &os, const Vector3<T> &v) {
        os << "[" << v.x << ", " << v.y << ", " << v.z << "]";
        return os;
    }
    Vector3<T> operator+(const Vector3<T> &v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }
    Vector3<T> &operator+=(const Vector3<T> &v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    Vector3<T> operator-(const Vector3<T> &v) const {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }
    Vector3<T> &operator-=(const Vector3<T> &v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    bool operator==(const Vector3<T> &v) const {
        return x == v.x && y == v.y && z == v.z;
    }
    bool operator!=(const Vector3<T> &v) const {
        return x != v.x || y != v.y || z != v.z;
    }
    Vector3<T> operator*(T s) const { return Vector3<T>(s * x, s * y, s * z); }
    Vector3<T> &operator*=(T s) {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }
    Vector3<T> operator/(T f) const {
        Assert(f != 0);
        Float inv = (Float)1 / f;
        return Vector3<T>(x * inv, y * inv, z * inv);
    }

    Vector3<T> &operator/=(T f) {
        Assert(f != 0);
        Float inv = (Float)1 / f;
        x *= inv;
        y *= inv;
        z *= inv;
        return *this;
    }
    Vector3<T> operator-() const { return Vector3<T>(-x, -y, -z); }
    Float LengthSquared() const { return x * x + y * y + z * z; }
    Float Length() const { return std::sqrt(LengthSquared()); }
    explicit Vector3(const Normal3<T> &n);

public:
    T x, y, z;
};

typedef Vector2<Float> Vector2f;
typedef Vector2<int> Vector2i;
typedef Vector3<Float> Vector3f;
typedef Vector3<int> Vector3i;

template <typename T> T MinComponent(const Vector3<T> &v) {
    return std::min(v.x, std::min(v.y, v.z));
}
template <typename T> T MaxComponent(const Vector3<T> &v) {
    return std::max(v.x, std::max(v.y, v.z));
}
template <typename T> int MaxDimension(const Vector3<T> &v) {
    return (v.x > v.y) ? ((v.x > v.z) ? 0 : 2) : ((v.y > v.z) ? 1 : 2);
}
template <typename T>
Vector3<T> Min(const Vector3<T> &p1, const Vector3<T> &p2) {
    return Vector3<T>(std::min(p1.x, p2.x), std::min(p1.y, p2.y),
                      std::min(p1.z, p2.z));
}
template <typename T>
Vector3<T> Max(const Vector3<T> &p1, const Vector3<T> &p2) {
    return Vector3<T>(std::max(p1.x, p2.x), std::max(p1.y, p2.y),
                      std::max(p1.z, p2.z));
}
template <typename T>
inline void CoordinateSystem(const Vector3<T> &v1, Vector3<T> *v2,
                             Vector3<T> *v3) {
    if (std::abs(v1.x) > std::abs(v1.y))
        *v2 = Vector3<T>(-v1.z, 0, v1.x) / std::sqrt(v1.x * v1.x + v1.z * v1.z);
    else
        *v2 = Vector3<T>(0, v1.z, -v1.y) / std::sqrt(v1.y * v1.y + v1.z * v1.z);
    *v3 = Cross(v1, *v2);
}

#endif // RAYTRACE_Vector_H