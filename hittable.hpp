//
// Created by XBY on 2021/11/27.
//

#ifndef RAYTRACE_HITTABLE_HPP
#define RAYTRACE_HITTABLE_HPP

#include "./ray.hpp"
#include "./vec3.hpp"
#include <cfloat>
#include <memory>
#include <vector>
using namespace std;

class BVHNode;
class material;

struct hit_record {
    float t;
    vec3 p, normal;
    shared_ptr<material> mat_ptr;
    bool front_face;

    inline void set_face_normal(const ray &r, const vec3 &outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    virtual bool hit(const ray &r, float t_min, float t_max,
                     hit_record &rec) const = 0;
    virtual bool bounding_box(float t0, float t1, BVHNode &box) const = 0;
};

class hittableList : public hittable {
public:
    hittableList() = default;
    void clear() { objects.clear(); }
    void add(shared_ptr<hittable> object) { objects.push_back(object); }
    virtual bool hit(const ray &r, float t_min, float t_max,
                     hit_record &rec) const override;
    virtual bool bounding_box(float t0, float t1, BVHNode &box) const override;
    vector<shared_ptr<hittable>> objects;
};

bool hittableList::hit(const ray &r, float t_min, float t_max,
                       hit_record &rec) const {
    hit_record tmp_rec;
    bool is_hit = false;
    double closest_so_far = t_max;
    for (int i = 0; i < objects.size(); i++) {
        if (objects[i]->hit(r, t_min, closest_so_far, tmp_rec)) {
            is_hit = true;
            closest_so_far = tmp_rec.t;
            rec = tmp_rec;
        }
    }
    return is_hit;
}

bool hittableList::bounding_box(float t0, float t1, BVHNode &box) const {
    return false;
}

#endif // RAYTRACE_HITTABLE_HPP
