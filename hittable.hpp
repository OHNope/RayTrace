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

class AABB;
class BVHNode;
class material;

struct hit_record {
    vec3 p, normal;
    float t, u, v;
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
    virtual bool bounding_box(float t0, float t1, AABB &box) const = 0;
};

class hittableList : public hittable {
public:
    hittableList() = default;
    hittableList(shared_ptr<hittable> object) { add(object); }
    void clear() { objects.clear(); }
    void add(shared_ptr<hittable> object) { objects.push_back(object); }
    virtual bool hit(const ray &r, float t_min, float t_max,
                     hit_record &rec) const override;
    virtual bool bounding_box(float t0, float t1, AABB &box) const override;
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

class flip_face : public hittable {
public:
    flip_face(shared_ptr<hittable> p) : ptr(p) {}

    virtual bool hit(const ray &r, float t_min, float t_max,
                     hit_record &rec) const {
        if (!ptr->hit(r, t_min, t_max, rec))
            return false;

        rec.front_face = !rec.front_face;
        return true;
    }

    virtual bool bounding_box(float t0, float t1, AABB &output_box) const {
        return ptr->bounding_box(t0, t1, output_box);
    }

public:
    shared_ptr<hittable> ptr; //指向几何物体
};

class translate : public hittable {
public:
    translate(shared_ptr<hittable> p, const vec3 &displacement)
        : ptr(p), offset(displacement) {}

    virtual bool hit(const ray &r, float t_min, float t_max,
                     hit_record &rec) const;
    virtual bool bounding_box(float t0, float t1, AABB &output_box) const;

public:
    shared_ptr<hittable> ptr;
    vec3 offset;
};

bool translate::hit(const ray &r, float t_min, float t_max,
                    hit_record &rec) const {
    ray moved_r(r.origin() - offset, r.direction(), r.time());
    if (!ptr->hit(moved_r, t_min, t_max, rec))
        return false;

    rec.p += offset;
    rec.set_face_normal(moved_r, rec.normal);

    return true;
}

class rotate_y : public hittable {
public:
    rotate_y(shared_ptr<hittable> p, double angle);

    virtual bool hit(const ray &r, float t_min, float t_max,
                     hit_record &rec) const;
    virtual bool bounding_box(float t0, float t1, AABB &output_box) const {
        output_box = bbox;
        return hasbox;
    }

public:
    shared_ptr<hittable> ptr;
    double sin_theta;
    double cos_theta;
    bool hasbox;
    AABB bbox;
};

#endif // RAYTRACE_HITTABLE_HPP
