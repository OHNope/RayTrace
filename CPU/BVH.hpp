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
        : BVHNode(list.objects, 0, list.objects.size() - 1, time0, time1) {}
    BVHNode(std::vector<shared_ptr<hittable>> &objects, int start, int end,
            double time0, double time1);
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

// SAH 优化构建 BVH
BVHNode::BVHNode(std::vector<shared_ptr<hittable>> &objects, int l, int r,
                 double time0, double time1) {
    if (l > r)
        return;

    // 不多于 n 个三角形 返回叶子节点
    if ((r - l + 1) == 1)
        left = right = objects[l];
    else {
        // 否则递归建树
        float Cost = DBL_MAX;
        int Axis = 0;
        int Split = (l + r) / 2;
        AABB tmpBox;
        for (int axis = 0; axis < 3; axis++) {
            // 分别按 x，y，z 轴排序
            auto comparator = (axis == 0)   ? box_x_compare
                              : (axis == 1) ? box_y_compare
                                            : box_z_compare;
            std::sort(objects.begin() + l, objects.begin() + r + 1, comparator);

            // leftMax[i]: [l, i] 中最大的 xyz 值
            // leftMin[i]: [l, i] 中最小的 xyz 值
            std::vector<vec3> leftMax(r - l + 1,
                                      vec3(DBL_MIN, DBL_MIN, DBL_MIN));
            std::vector<vec3> leftMin(r - l + 1,
                                      vec3(DBL_MAX, DBL_MAX, DBL_MAX));
            // 计算前缀 注意 i-l 以对齐到下标 0
            for (int i = l; i <= r; i++) {
                shared_ptr<hittable> t = objects[i];
                int bias = (i == l) ? 0 : 1; // 第一个元素特殊处理
                tmpBox =
                    t->bounding_box(time0, time1, tmpBox) ? tmpBox : AABB();
                leftMax[i - l][0] =
                    fmax(leftMax[i - l - bias][0], tmpBox.max()[0]);
                leftMax[i - l][1] =
                    fmax(leftMax[i - l - bias][1], tmpBox.max()[1]);
                leftMax[i - l][2] =
                    fmax(leftMax[i - l - bias][2], tmpBox.max()[2]);

                leftMin[i - l][0] =
                    fmin(leftMin[i - l - bias][0], tmpBox.min()[0]);
                leftMin[i - l][1] =
                    fmin(leftMin[i - l - bias][1], tmpBox.min()[1]);
                leftMin[i - l][2] =
                    fmin(leftMin[i - l - bias][2], tmpBox.min()[2]);
            }

            // rightMax[i]: [i, r] 中最大的 xyz 值
            // rightMin[i]: [i, r] 中最小的 xyz 值
            std::vector<vec3> rightMax(r - l + 1,
                                       vec3(DBL_MIN, DBL_MIN, DBL_MIN));
            std::vector<vec3> rightMin(r - l + 1,
                                       vec3(DBL_MAX, DBL_MAX, DBL_MAX));
            // 计算后缀 注意 i-l 以对齐到下标 0
            for (int i = r; i >= l; i--) {
                shared_ptr<hittable> t = objects[i];
                int bias = (i == r) ? 0 : 1; // 第一个元素特殊处理
                tmpBox =
                    t->bounding_box(time0, time1, tmpBox) ? tmpBox : AABB();
                rightMax[i - l][0] =
                    fmax(rightMax[i - l + bias][0], tmpBox.max()[0]);
                rightMax[i - l][1] =
                    fmax(rightMax[i - l + bias][1], tmpBox.max()[1]);
                rightMax[i - l][2] =
                    fmax(rightMax[i - l + bias][2], tmpBox.max()[2]);

                rightMin[i - l][0] =
                    fmin(rightMin[i - l + bias][0], tmpBox.min()[0]);
                rightMin[i - l][1] =
                    fmin(rightMin[i - l + bias][1], tmpBox.min()[1]);
                rightMin[i - l][2] =
                    fmin(rightMin[i - l + bias][2], tmpBox.min()[2]);
            }

            // 遍历寻找分割
            float cost = DBL_MAX;
            int split = l;
            for (int i = l; i <= r - 1; i++) {
                float lenx, leny, lenz;
                // 左侧 [l, i]
                vec3 leftAA = leftMin[i - l];
                vec3 leftBB = leftMax[i - l];
                lenx = leftBB[0] - leftAA[0];
                leny = leftBB[1] - leftAA[1];
                lenz = leftBB[2] - leftAA[2];
                float leftS =
                    2.0 * ((lenx * leny) + (lenx * lenz) + (leny * lenz));
                float leftCost = leftS * (i - l + 1);

                // 右侧 [i+1, r]
                vec3 rightAA = rightMin[i + 1 - l];
                vec3 rightBB = rightMax[i + 1 - l];
                lenx = rightBB[0] - rightAA[0];
                leny = rightBB[1] - rightAA[1];
                lenz = rightBB[2] - rightAA[2];
                float rightS =
                    2.0 * ((lenx * leny) + (lenx * lenz) + (leny * lenz));
                float rightCost = rightS * (r - i);

                // 记录每个分割的最小答案
                float totalCost = leftCost + rightCost;
                if (totalCost < cost) {
                    cost = totalCost;
                    split = i;
                }
            }
            // 记录每个轴的最佳答案
            if (cost < Cost) {
                Cost = cost;
                Axis = axis;
                Split = split;
            }
        }

        // 按最佳轴分割
        auto comparator = (Axis == 0)   ? box_x_compare
                          : (Axis == 1) ? box_y_compare
                                        : box_z_compare;
        std::sort(objects.begin() + l, objects.begin() + r + 1, comparator);

        // 递归
        left = make_shared<BVHNode>(objects, l, Split, time0, time1);
        right = make_shared<BVHNode>(objects, Split + 1, r, time0, time1);
    }
    AABB box_left, box_right;

    if (!left->bounding_box(time0, time1, box_left) ||
        !right->bounding_box(time0, time1, box_right))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    box = surrounding_box(box_left, box_right);
}

#endif // RAYTRACE_BVH_HPP
