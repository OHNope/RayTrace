//
// Created by XBY on 2021/12/19.
//

#ifndef RAYTRACE_CONSTANT_MEDIUM_HPP
#define RAYTRACE_CONSTANT_MEDIUM_HPP

#include "./material.hpp"

class constant_medium : public hittable {
public:
    constant_medium(shared_ptr<hittable> b, double d, shared_ptr<texture> a)
        : boundary(b), neg_inv_density(-1 / d) {
        phase_function = make_shared<isotropic>(a);
    }

    virtual bool hit(const ray &r, float t_min, float t_max,
                     hit_record &rec) const;

    virtual bool bounding_box(float t0, float t1, AABB &output_box) const {
        return boundary->bounding_box(t0, t1, output_box);
    }

public:
    shared_ptr<hittable> boundary; // hittable物体表示体积体边界
    shared_ptr<material> phase_function;
    double neg_inv_density;
};

bool constant_medium::hit(const ray &r, float t_min, float t_max,
                          hit_record &rec) const {
    // Print occasional samples when debugging. To enable, set enableDebug true.
    const bool enableDebug = false;
    const bool debugging = enableDebug && random_double() < 0.00001;

    hit_record rec1, rec2;

    if (!boundary->hit(r, FLT_MIN, FLT_MAX, rec1))
        return false;

    if (!boundary->hit(r, rec1.t + 0.0001, FLT_MAX, rec2))
        return false;

    if (debugging)
        std::cerr << "\nt0=" << rec1.t << ", t1=" << rec2.t << '\n';

    if (rec1.t < t_min)
        rec1.t = t_min;
    if (rec2.t > t_max)
        rec2.t = t_max;

    if (rec1.t >= rec2.t)
        return false;

    if (rec1.t < 0)
        rec1.t = 0;

    const auto ray_length = r.direction().length();
    const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
    const auto hit_distance = neg_inv_density * log(random_double());

    if (hit_distance > distance_inside_boundary)
        return false;

    rec.t = rec1.t + hit_distance / ray_length;
    rec.p = r.point_at(rec.t);

    if (debugging) {
        /*std::cerr << "hit_distance = " << hit_distance << '\n'
                  << "rec.t = " << rec.t << '\n'
                  << "rec.p = " << rec.p << '\n';*/
    }

    rec.normal = vec3(1, 0, 0); // arbitrary
    rec.front_face = true;      // also arbitrary
    rec.mat_ptr = phase_function;

    return true;
}

#endif // RAYTRACE_CONSTANT_MEDIUM_HPP
