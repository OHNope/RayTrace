//
// Created by XBY on 2021/12/18.
//

#ifndef RAYTRACE_BOX_HPP
#define RAYTRACE_BOX_HPP

#include "rect.hpp"

class box : public hittable {
public:
    box() {}
    box(const vec3 &p0, const vec3 &p1, shared_ptr<material> ptr);

    virtual bool hit(const ray &r, float t0, float t1, hit_record &rec) const;

    virtual bool bounding_box(float t0, float t1, AABB &output_box) const {
        output_box = AABB(box_min, box_max);
        return true;
    }

public:
    vec3 box_min;
    vec3 box_max;
    hittableList sides;
};

box::box(const vec3 &p0, const vec3 &p1, shared_ptr<material> ptr) {
    box_min = p0;
    box_max = p1;

    sides.add(
        make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
    sides.add(make_shared<flip_face>(
        make_shared<xy_rect>(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr)));

    sides.add(
        make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
    sides.add(make_shared<flip_face>(
        make_shared<xz_rect>(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr)));

    sides.add(
        make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
    sides.add(make_shared<flip_face>(
        make_shared<yz_rect>(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr)));
}

bool box::hit(const ray &r, float t0, float t1, hit_record &rec) const {
    return sides.hit(r, t0, t1, rec);
}
#endif // RAYTRACE_BOX_HPP
