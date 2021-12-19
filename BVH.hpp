//
// Created by XBY on 2021/12/5.
//

#ifndef RAYTRACE_BVH_HPP
#define RAYTRACE_BVH_HPP

#include "./AABB.hpp"
#include "./externalTools.hpp"
#include "./hittable.hpp"
#include "./ray.hpp"
#include "./vec3.hpp"
#include <algorithm>
#include <cfloat>
#include <iostream>
#include <vector>
using namespace std;

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

#endif // RAYTRACE_BVH_HPP
