//
// Created by XBY on 2021/12/5.
//

#ifndef RAYTRACE_BVH_HPP
#define RAYTRACE_BVH_HPP

#include "./externalTools.hpp"
#include "./hittable.hpp"
#include "./ray.hpp"
#include "./vec3.hpp"
#include <algorithm>
#include <cfloat>
#include <iostream>
#include <vector>
using namespace std;

class AABB {
public:
    AABB() = default;
    AABB(const vec3 &a, const vec3 &b) {
        _min = a;
        _max = b;
    }

    vec3 min() const { return _min; }
    vec3 max() const { return _max; }

    bool hit(const ray &r, double tmin, double tmax) const;
    vec3 _min;
    vec3 _max;
};
inline bool AABB::hit(const ray &r, double tmin, double tmax) const {
    for (int a = 0; a < 3; a++) {
        auto invD = 1.0f / r.direction()[a];
        auto t0 = (min()[a] - r.origin()[a]) * invD;
        auto t1 = (max()[a] - r.origin()[a]) * invD;
        if (invD < 0.0f)
            std::swap(t0, t1);
        tmin = t0 > tmin ? t0 : tmin;
        tmax = t1 < tmax ? t1 : tmax;
        if (tmax <= tmin)
            return false;
    }
    return true;
}

AABB surrounding_box(AABB box0, AABB box1) {
    vec3 small(fmin(box0.min().x(), box1.min().x()),
               fmin(box0.min().y(), box1.min().y()),
               fmin(box0.min().z(), box1.min().z()));
    vec3 big(fmax(box0.max().x(), box1.max().x()),
             fmax(box0.max().y(), box1.max().y()),
             fmax(box0.max().z(), box1.max().z()));
    return AABB(small, big);
}

// BVH 树节点
class BVHNode : public hittable {
public:
    shared_ptr<hittable> left;
    shared_ptr<hittable> right;
    AABB box;
    BVHNode() = default;
    BVHNode(hittableList &list, double time0, double time1)
        : BVHNode(list.objects, 0, list.objects.size(), time0, time1) {}
    BVHNode(std::vector<shared_ptr<hittable>> &objects, size_t start,
            size_t end, double time0, double time1);
    virtual bool hit(const ray &r, float tmin, float tmax,
                     hit_record &rec) const override;
    virtual bool bounding_box(float t0, float t1,
                              AABB &output_box) const override;
};

bool BVHNode::bounding_box(float t0, float t1, AABB &output_box) const {
    output_box = box;
    return true;
}

bool BVHNode::hit(const ray &r, float t_min, float t_max,
                  hit_record &rec) const {
    if (!box.hit(r, t_min, t_max))
        return false;

    bool hit_left = left->hit(r, t_min, t_max, rec);
    bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);

    return hit_left || hit_right;
}

inline bool box_compare(const shared_ptr<hittable> a,
                        const shared_ptr<hittable> b, int axis) {
    AABB box_a;
    AABB box_b;

    if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    return box_a.min().e[axis] < box_b.min().e[axis];
}

bool box_x_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return box_compare(a, b, 0);
}

bool box_y_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return box_compare(a, b, 1);
}

bool box_z_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
    return box_compare(a, b, 2);
}

BVHNode::BVHNode(std::vector<shared_ptr<hittable>> &objects, size_t start,
                 size_t end, double time0, double time1) {
    int axis = random_int(0, 2);
    auto comparator = (axis == 0)   ? box_x_compare
                      : (axis == 1) ? box_y_compare
                                    : box_z_compare;

    size_t object_span = end - start;

    if (object_span == 1) {
        left = right = objects[start];
    } else if (object_span == 2) {
        if (comparator(objects[start], objects[start + 1])) {
            left = objects[start];
            right = objects[start + 1];
        } else {
            left = objects[start + 1];
            right = objects[start];
        }
    } else {
        std::sort(objects.begin() + start, objects.begin() + end, comparator);

        auto mid = start + object_span / 2;
        left = make_shared<BVHNode>(objects, start, mid, time0, time1);
        right = make_shared<BVHNode>(objects, mid, end, time0, time1);
    }

    AABB box_left, box_right;

    if (!left->bounding_box(time0, time1, box_left) ||
        !right->bounding_box(time0, time1, box_right))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    box = surrounding_box(box_left, box_right);
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
#endif // RAYTRACE_BVH_HPP
