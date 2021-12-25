//
// Created by XBY on 2021/11/27.
//

#ifndef RAYTRACE_HITTABLE_HPP
#define RAYTRACE_HITTABLE_HPP

#include "./AABB.hpp"
#include "./ray.hpp"
#include "./texture.hpp"
#include "./vec3.hpp"
#include <cfloat>
#include <memory>
#include <vector>
using namespace std;

class material;
class isotropic;

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

bool hittableList::bounding_box(float t0, float t1, AABB &output_box) const {
    if (objects.empty())
        return false;

    AABB temp_box;
    bool first_box = true;

    for (const auto &object : objects) {
        if (!object->bounding_box(t0, t1, temp_box))
            return false;
        output_box =
            first_box ? temp_box : surrounding_box(output_box, temp_box);
        first_box = false;
    }

    return true;
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

bool translate::bounding_box(float t0, float t1, AABB &output_box) const {
    if (!ptr->bounding_box(t0, t1, output_box))
        return false;
    output_box = AABB(output_box.min() + offset, output_box.max() + offset);
    return true;
}

class rotate_y : public hittable {
public:
    rotate_y(shared_ptr<hittable> p, double angle);

    virtual bool hit(const ray &r, float t_min, float t_max,
                     hit_record &rec) const;
    virtual bool bounding_box(float t0, float t1, AABB &output_box) const;

public:
    shared_ptr<hittable> ptr;
    double sin_theta;
    double cos_theta;
    bool hasbox;
    AABB bbox;
};

bool rotate_y::bounding_box(float t0, float t1, AABB &output_box) const {
    output_box = bbox;
    return hasbox;
}

rotate_y::rotate_y(shared_ptr<hittable> p, double angle) : ptr(p) {
    auto radians = degrees_to_radians(angle);
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    hasbox = ptr->bounding_box(0, 1, bbox);

    vec3 min(DBL_MAX, DBL_MAX, DBL_MAX);
    vec3 max(DBL_MIN, DBL_MIN, DBL_MIN);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                auto x = i * bbox.max().x() + (1 - i) * bbox.min().x();
                auto y = j * bbox.max().y() + (1 - j) * bbox.min().y();
                auto z = k * bbox.max().z() + (1 - k) * bbox.min().z();

                auto newx = cos_theta * x + sin_theta * z;
                auto newz = -sin_theta * x + cos_theta * z;

                vec3 tester(newx, y, newz);

                for (int c = 0; c < 3; c++) {
                    min[c] = fmin(min[c], tester[c]);
                    max[c] = fmax(max[c], tester[c]);
                }
            }
        }
    }

    bbox = AABB(min, max);
}

bool rotate_y::hit(const ray &r, float t_min, float t_max,
                   hit_record &rec) const {
    vec3 origin = r.origin();
    vec3 direction = r.direction();

    origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
    origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

    direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
    direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

    ray rotated_r(origin, direction, r.time());

    if (!ptr->hit(rotated_r, t_min, t_max, rec))
        return false;

    vec3 p = rec.p;
    vec3 normal = rec.normal;

    p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
    p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

    normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
    normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

    rec.p = p;
    rec.set_face_normal(rotated_r, normal);

    return true;
}

#endif // RAYTRACE_HITTABLE_HPP
