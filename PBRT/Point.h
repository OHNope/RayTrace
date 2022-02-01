//
// Created by XBY on 2022/1/29.
//

#ifndef RAYTRACE_POINT_H
#define RAYTRACE_POINT_H

template <typename T> class Point2 {
public:
    explicit Point2(const Point3<T> &p) : x(p.x), y(p.y) { Assert(!HasNaNs()); }
    Point2() { x = y = 0; }
    Point2(T xx, T yy) : x(xx), y(yy) { Assert(!HasNaNs()); }

    template <typename U> explicit Point2(const Point2<U> &p) {
        x = (T)p.x;
        y = (T)p.y;
        Assert(!HasNaNs());
    }

    template <typename U> explicit Point2(const Vector2<U> &p) {
        x = (T)p.x;
        y = (T)p.y;
        Assert(!HasNaNs());
    }

    template <typename U> explicit operator Vector2<U>() const {
        return Vector2<U>(x, y);
    }

#ifndef NDEBUG
    Point2(const Point2<T> &p) {
        Assert(!p.HasNaNs());
        x = p.x;
        y = p.y;
    }

    Point2<T> &operator=(const Point2<T> &p) {
        Assert(!p.HasNaNs());
        x = p.x;
        y = p.y;
        return *this;
    }
#endif // !NDEBUG
    friend std::ostream &operator<<(std::ostream &os, const Point2<T> &p) {
        os << "[" << p.x << ", " << p.y << "]";
        return os;
    }

    Point2<T> operator+(const Vector2<T> &v) const {
        Assert(!v.HasNaNs());
        return Point2<T>(x + v.x, y + v.y);
    }

    Point2<T> &operator+=(const Vector2<T> &v) {
        Assert(!v.HasNaNs());
        x += v.x;
        y += v.y;
        return *this;
    }
    Vector2<T> operator-(const Point2<T> &p) const {
        Assert(!p.HasNaNs());
        return Vector2<T>(x - p.x, y - p.y);
    }

    Point2<T> operator-(const Vector2<T> &v) const {
        Assert(!v.HasNaNs());
        return Point2<T>(x - v.x, y - v.y);
    }
    Point2<T> operator-() const { return Point2<T>(-x, -y); }
    Point2<T> &operator-=(const Vector2<T> &v) {
        Assert(!v.HasNaNs());
        x -= v.x;
        y -= v.y;
        return *this;
    }
    Point2<T> &operator+=(const Point2<T> &p) {
        Assert(!p.HasNaNs());
        x += p.x;
        y += p.y;
        return *this;
    }
    Point2<T> operator+(const Point2<T> &p) const {
        Assert(!p.HasNaNs());
        return Point2<T>(x + p.x, y + p.y);
    }
    Point2<T> operator*(T f) const { return Point2<T>(f * x, f * y); }
    Point2<T> &operator*=(T f) {
        x *= f;
        y *= f;
        return *this;
    }
    Point2<T> operator/(T f) const {
        Float inv = (Float)1 / f;
        return Point2<T>(inv * x, inv * y);
    }
    Point2<T> &operator/=(T f) {
        Float inv = (Float)1 / f;
        x *= inv;
        y *= inv;
        return *this;
    }
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
    bool operator==(const Point2<T> &p) const { return x == p.x && y == p.y; }
    bool operator!=(const Point2<T> &p) const { return x != p.x || y != p.y; }
    bool HasNaNs() const { return std::isnan(x) || std::isnan(y); }

    T x, y;
};

typedef Point2<Float> Point2f;
typedef Point2<int> Point2i;
typedef Point3<Float> Point3f;
typedef Point3<int> Point3i;

#endif // RAYTRACE_POINT_H
