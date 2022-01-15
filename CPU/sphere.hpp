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
    virtual double pdf_value(const point3 &o, const vec3 &v) const override;
    virtual vec3 random(const point3 &o) const override;

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
    auto theta = asinFast4(p.y());
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

double sphere::pdf_value(const point3 &o, const vec3 &v) const {
    hit_record rec;
    if (!this->hit(ray(o, v), 0.001, FLT_MAX, rec))
        return 0;

    auto cos_theta_max =
        sqrt(1 - radius * radius / (centre - o).length_squared());
    auto solid_angle = 2 * M_PI * (1 - cos_theta_max);

    return 1 / solid_angle;
}

inline vec3 random_to_sphere(double radius, double distance_squared) {
    auto r1 = random_double();
    auto r2 = random_double();
    auto z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);

    auto phi = 2 * M_PI * r1;
    auto x = cos(phi) * sqrt(1 - z * z);
    auto y = sin(phi) * sqrt(1 - z * z);

    return vec3(x, y, z);
}

vec3 sphere::random(const point3 &o) const {
    vec3 direction = centre - o;
    auto distance_squared = direction.length_squared();
    onb uvw;
    uvw.build_from_w(direction);
    return uvw.local(random_to_sphere(radius, distance_squared));
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
