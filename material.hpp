//
// Created by XBY on 2021/11/27.
//

#ifndef RAYTRACE_MATERIAL_HPP
#define RAYTRACE_MATERIAL_HPP

#include "./externalTools.hpp"
#include "./hittable.hpp"
#include "./ray.hpp"
#include "./texture.hpp"

class material {
public:
    virtual bool scatter(const ray &r_in, const hit_record &rec,
                         vec3 &attenuation, ray &scattered) const = 0;
};

class lambertian : public material {
public:
    lambertian(shared_ptr<texture> a) : albedo(a){};
    virtual bool scatter(const ray &r_in, const hit_record &rec,
                         vec3 &attenuation, ray &scattered) const override {
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        scattered = ray(rec.p, target - rec.p, r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    };
    shared_ptr<texture> albedo;
};

vec3 reflect(const vec3 &v, const vec3 &n) {
    return v - 2 * dot(v, n) * n;
} //反射

class metal : public material {
public:
    metal(const vec3 &a, float f) : albedo(a) { fuzz = f < 1 ? f : 1; };
    virtual bool scatter(const ray &r_in, const hit_record &rec,
                         vec3 &attenuation, ray &scattered) const override {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    };
    vec3 albedo;
    float fuzz; //反射可失真程度
};

bool refract(const vec3 &v, const vec3 &n, float snell_n,
             vec3 &refracted) { //折射
    vec3 unit_v = unit_vector(v);
    float dt = dot(unit_v, n);
    float discriminant = 1.0 - snell_n * snell_n * (1 - dt * dt);
    if (discriminant < 0)
        return false;
    refracted = snell_n * (unit_v - n * dt) - n * sqrt(discriminant);
    return true;
}

float schlick(float cosi, float cost, float etai,
              float etat) { //导体菲涅尔近似方程
    float r0 = (etai - etat) / (etai + etat);
    r0 *= r0;
    float a = 1.0f - (etai < etat ? cosi : cost);
    float aa = a * a;
    return r0 + (1.0f - r0) * aa * aa * a;
}

class dielectric : public material {
public:
    dielectric(float ri) : ref_idx(ri){};
    virtual bool scatter(const ray &r_in, const hit_record &rec,
                         vec3 &attenuation, ray &scattered) const override {
        vec3 outward_normal;
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        float snell_n;
        attenuation = vec3(1.0, 1.0, 1.0);
        vec3 refracted;
        float reflect_prob;
        float cosine;
        bool isOut =
            dot(r_in.direction(), rec.normal) > 0; //入射光线与法线呈锐角
        if (isOut) {                               //从介质入射到空气
            outward_normal = -rec.normal;
            snell_n = ref_idx;
        } else { //从空气入射到介质
            outward_normal = rec.normal;
            snell_n = 1.0 / ref_idx;
        }
        bool isRefract = refract(r_in.direction(), outward_normal, snell_n,
                                 refracted); //计算折射光线向量
        cosine = isOut ? dot(refracted, rec.normal)
                       : -(dot(r_in.direction(), rec.normal) /
                           r_in.direction().length());
        if (isRefract)
            // scattered = ray(rec.p, refracted);
            reflect_prob = reflectance(cosine, ref_idx);
        else
            reflect_prob = 1.0;
        if (get_random() < reflect_prob)
            scattered = ray(rec.p, reflected);
        else
            scattered = ray(rec.p, refracted);
        return true;
    };
    float ref_idx;

private:
    static double reflectance(double cosine, double ref_idx) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};
#endif // RAYTRACE_MATERIAL_HPP
