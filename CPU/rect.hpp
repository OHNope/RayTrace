//
// Created by XBY on 2021/12/18.
//

#ifndef RAYTRACE_RECT_HPP
#define RAYTRACE_RECT_HPP

enum Rect_Face { XY, XZ, YZ };

template <Rect_Face N> class Rect : public hittable {
public:
    Rect() = default;
    Rect(double _a0, double _a1, double _b0, double _b1, double _k,
         shared_ptr<material> mat)
        : a0(_a0), a1(_a1), b0(_b0), b1(_b1), k(_k), mp(mat){};
    virtual bool hit(const ray &r, float t0, float t1, hit_record &rec) const {
        double origin_x = r.origin().x(), origin_y = r.origin().y(),
               origin_z = r.origin().z();
        double direct_x = r.direction().x(), direct_y = r.direction().y(),
               direct_z = r.direction().z();
        double t, a, b;
        vec3 outward_normal;
        if constexpr (N == XY) {
            t = (k - origin_z) / direct_z;
            a = origin_x + t * direct_x;
            b = origin_y + t * direct_y;
            outward_normal = vec3(0, 0, 1);
        } else if constexpr (N == XZ) {
            t = (k - origin_y) / direct_y;
            a = origin_x + t * direct_x;
            b = origin_z + t * direct_z;
            outward_normal = vec3(0, 1, 0);
        } else {
            t = (k - origin_x) / direct_x;
            a = origin_y + t * direct_y;
            b = origin_z + t * direct_z;
            outward_normal = vec3(1, 0, 0);
        }

        if (t < t0 || t > t1)
            return false;
        if (a < a0 || a > a1 || b < b0 || b > b1)
            return false;
        rec.u = (a - a0) / (a1 - a0);
        rec.v = (b - b0) / (b1 - b0);
        rec.t = t;
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mp;
        rec.p = r.point_at(t);
        return true;
    };

    virtual bool bounding_box(float t0, float t1, AABB &output_box) const {
        if constexpr (N == XY)
            output_box =
                AABB(vec3(a0, b0, k - 0.0001), vec3(a1, b1, k + 0.0001));
        else if constexpr (N == XZ)
            output_box =
                AABB(vec3(a0, k - 0.0001, b0), vec3(a1, k + 0.0001, b1));
        else
            output_box =
                AABB(vec3(k - 0.0001, a0, b0), vec3(k + 0.0001, a1, b1));
        return true;
    }

public:
    shared_ptr<material> mp;
    double a0, a1, b0, b1, k;
};

#endif // RAYTRACE_RECT_HPP
