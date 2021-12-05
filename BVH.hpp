//
// Created by XBY on 2021/12/5.
//

#ifndef RAYTRACE_BVH_HPP
#define RAYTRACE_BVH_HPP

#include "./sphere.hpp"
#include "./vec3.hpp"
#include <algorithm>
#include <cfloat>
#include <vector>
using namespace std;

// BVH 树节点
struct BVHNode {
    BVHNode *left = NULL; // 左右子树索引
    BVHNode *right = NULL;
    int n, index; // 叶子节点信息
    vec3 AA, BB;  // 碰撞盒
    BVHNode(const vec3 &a, const vec3 &b) {
        AA = a;
        BB = b;
    }
};

bool sphere::bounding_box(float t0, float t1, BVHNode &box) const {
    box = BVHNode(centre - vec3(radius, radius, radius),
                  centre + vec3(radius, radius, radius));
    return true;
}

#endif // RAYTRACE_BVH_HPP
