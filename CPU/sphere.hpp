//
// Created by XBY on 2021/11/27.
//

#ifndef RAYTRACE_SPHERE_HPP
#define RAYTRACE_SPHERE_HPP

#include "./BVH.hpp"
#include "./hittable.hpp"
#include "./ray.hpp"
#include "./vec3.hpp"

class BVHNode;

class sphere : public hittable {
public:
    sphere();
    sphere(vec3 cen, float r, shared_ptr<material> m)
        : centre(cen), radius(r), mat_ptr(m){};
    virtual bool hit(const ray &r, float t_min, float t_max,
                     hit_record &rec) const override;
    bool bounding_box(float t0, float t1, AABB &box) const override;

private:
    vec3 centre;
    float radius;
    shared_ptr<material> mat_ptr;
};

bool sphere::bounding_box(float t0, float t1, AABB &box) const {
    box = AABB(centre - vec3(radius, radius, radius),
               centre + vec3(radius, radius, radius));
    return true;
}

void get_sphere_uv(const vec3 &p, float &u, float &v) {
    auto phi = atan2(p.z(), p.x());
    auto theta = asin(p.y());
    u = 1 - (phi + M_PI) / (2 * M_PI);
    v = (theta + M_PI / 2) / M_PI;
}

bool sphere::hit(const ray &r, float t_min, float t_max,
                 hit_record &rec) const {
    vec3 oc = r.origin() - centre;
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - a * c;
    if (discriminant < 0)
        return false;

    float sqrt_delta = sqrt(discriminant);

    float tmp = (-b - sqrt_delta) / a;
    if (tmp > t_max || tmp < t_min || fabs(tmp) < 0.0005f) {
        tmp = (-b + sqrt_delta) / a;
        if (tmp > t_max || tmp < t_min || fabs(tmp) < 0.0005f)
            return false;
    }
    rec.t = tmp;
    rec.p = r.point_at(rec.t);
    rec.normal = (rec.p - centre) / radius;
    rec.mat_ptr = mat_ptr;
    get_sphere_uv((rec.p - centre) / radius, rec.u, rec.v);
    return true;
}

// 三角形
class Triangle : public hittable {
public:
    Triangle(vec3 P1, vec3 P2, vec3 P3, shared_ptr<material> m)
        : p1(P1), p2(P2), p3(P3), mat_ptr(m) {
        center = (p1 + p2 + p3) / vec3(3, 3, 3);
    }
    vec3 p1, p2, p3, center; // 三顶点
    shared_ptr<material> mat_ptr;

    // 与光线求交
    virtual bool hit(const ray &r, float t_min, float t_max,
                     hit_record &rec) const override;
    virtual bool bounding_box(float t0, float t1, AABB &box) const override;
};

bool Triangle::hit(const ray &r, float t_min, float t_max,
                   hit_record &rec) const {

    vec3 S = r.origin();              // 射线起点
    vec3 d = r.direction();           // 射线方向
    vec3 n = cross(p2 - p1, p3 - p1); // 法向量
    vec3 N = n;
    if (dot(N, d) > 0.0f)
        N = -N; // 获取正确的法向量

    // 如果视线和三角形平行
    if (fabs(dot(N, d)) < 0.00001f)
        return false;

    // 距离
    float t = (dot(N, p1) - dot(S, N)) / dot(d, N);
    if (t < 0.0005f)
        return false; // 如果三角形在相机背面,防止击中自己

    // 交点计算
    vec3 P = S + d * t;

    // 判断交点是否在三角形中
    vec3 c1 = cross(p2 - p1, P - p1);
    vec3 c2 = cross(p3 - p2, P - p2);
    vec3 c3 = cross(p1 - p3, P - p3);
    if (dot(c1, n) < 0 || dot(c2, n) < 0 || dot(c3, n) < 0)
        return false;

    // 装填返回结果
    rec.t = t;
    rec.p = P;
    rec.mat_ptr = this->mat_ptr;
    rec.normal = N; // 要返回正确的法向
    return true;
}

bool Triangle::bounding_box(float t0, float t1, AABB &box) const {
    return false;
}

bool cmpx(const Triangle &t1, const Triangle &t2) {
    return t1.center[0] < t2.center[0];
}
bool cmpy(const Triangle &t1, const Triangle &t2) {
    return t1.center[1] < t2.center[1];
}
bool cmpz(const Triangle &t1, const Triangle &t2) {
    return t1.center[2] < t2.center[2];
}

class moving_sphere : public hittable {
public:
    moving_sphere() {}
    moving_sphere(vec3 cen0, vec3 cen1, double t0, double t1, double r,
                  shared_ptr<material> m)
        : center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r),
          mat_ptr(m){};

    virtual bool hit(const ray &r, float t_min, float t_max,
                     hit_record &rec) const override;
    virtual bool bounding_box(float t0, float t1, AABB &box) const override;

    vec3 center(double time) const;

    vec3 center0, center1;
    double time0, time1;
    double radius;
    shared_ptr<material> mat_ptr;
};

vec3 moving_sphere::center(double time) const {
    return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}

bool moving_sphere::hit(const ray &r, float t_min, float t_max,
                        hit_record &rec) const {
    vec3 oc = r.origin() - center(r.time());
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = half_b * half_b - a * c;

    if (discriminant > 0) {
        auto root = sqrt(discriminant);

        auto temp = (-half_b - root) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at(rec.t);
            vec3 outward_normal = (rec.p - center(r.time())) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            return true;
        }

        temp = (-half_b + root) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.p = r.point_at(rec.t);
            vec3 outward_normal = (rec.p - center(r.time())) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}

bool moving_sphere::bounding_box(float t0, float t1, AABB &output_box) const {
    AABB box0(center(t0) - vec3(radius, radius, radius),
              center(t0) + vec3(radius, radius, radius));
    AABB box1(center(t1) - vec3(radius, radius, radius),
              center(t1) + vec3(radius, radius, radius));
    output_box = surrounding_box(box0, box1);
    return true;
}

#endif // RAYTRACE_SPHERE_HPP