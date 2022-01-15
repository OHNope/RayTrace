//
// Created by XBY on 2021/11/27.
//

#ifndef RAYTRACE_MATERIAL_HPP
#define RAYTRACE_MATERIAL_HPP

#include <memory>

#include "./ONB.hpp"
#include "./externalTools.hpp"
#include "./hittable.hpp"
#include "./ray.hpp"
#include "./texture.hpp"
#include "PDF.hpp"

struct scatter_record {
    ray specular_ray;
    bool is_specular;
    color attenuation;
    shared_ptr<pdf> pdf_ptr;
};

class material {
public:
    virtual color emitted(const ray &r_in, const hit_record &rec, double u,
                          double v, const point3 &p) const {
        return color(0, 0, 0);
    }

    virtual bool scatter(const ray &r_in, const hit_record &rec,
                         scatter_record &srec) const {
        return false;
    }

    virtual double scattering_pdf(const ray &r_in, const hit_record &rec,
                                  const ray &scattered) const {
        return 0;
    }
};

class lambertian : public material {
public:
    lambertian(const color &a) : albedo(make_shared<constant_texture>(a)) {}
    lambertian(shared_ptr<texture> a) : albedo(a){};
    virtual bool scatter(const ray &r_in, const hit_record &rec,
                         scatter_record &srec) const override {
        srec.is_specular = false;
        srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
        srec.pdf_ptr = std::make_shared<cosine_pdf>(rec.normal);
        return true;
    }
    double scattering_pdf(const ray &r_in, const hit_record &rec,
                          const ray &scattered) const override {
        auto cosine = dot(rec.normal, unit_vector(scattered.direction()));
        return cosine < 0 ? 0 : cosine / M_PI;
    }
    shared_ptr<texture> albedo;
};

inline vec3 reflect(const vec3 &v, const vec3 &n) {
    return v - 2 * dot(v, n) * n;
}

inline vec3 refract(const vec3 &uv, const vec3 &n, double etai_over_etat) {
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3 r_out_parallel =
        -1.0 / InvSqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

class metal : public material {
public:
    metal(const vec3 &a, float f) : albedo(a) { fuzz = f < 1 ? f : 1; };
    virtual bool scatter(const ray &r_in, const hit_record &rec,
                         scatter_record &srec) const override {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        srec.specular_ray =
            ray(rec.p, reflected + fuzz * random_in_unit_sphere());
        srec.attenuation = albedo;
        srec.is_specular = true;
        srec.pdf_ptr = nullptr;
        return true;
    }
    vec3 albedo;
    float fuzz; //反射可失真程度
};

float schlick(float cosi, float cost, float etai,
              float etat) { //导体菲涅尔近似方程
    float r0 = (etai - etat) / (etai + etat);
    r0 *= r0;
    float a = 1.0f - (etai < etat ? cosi : cost);
    float aa = a * a;
    return r0 + (1.0f - r0) * aa * aa * a;
}

bool refract(const vec3 &v, const vec3 &n, float snell_n,
             vec3 &refracted) { //折射
    vec3 unit_v = unit_vector(v);
    float dt = dot(unit_v, n);
    float discriminant = 1.0 - snell_n * snell_n * (1 - dt * dt);
    if (discriminant < 0)
        return false;
    refracted = snell_n * (unit_v - n * dt) - n * 1.0 / InvSqrt(discriminant);
    return true;
}

class dielectric : public material {
public:
    dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    virtual bool scatter(const ray &r_in, const hit_record &rec,
                         scatter_record &srec) const override {
        vec3 outward_normal;
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        float snell_n;
        srec.is_specular = true;
        srec.pdf_ptr = nullptr;
        srec.attenuation = color(1.0, 1.0, 1.0);
        vec3 refracted;
        float reflect_prob;
        float cosine;
        bool isOut =
            dot(r_in.direction(), rec.normal) > 0; //入射光线与法线呈锐角
        if (isOut) {                               //从介质入射到空气
            outward_normal = -rec.normal;
            snell_n = ir;
        } else { //从空气入射到介质
            outward_normal = rec.normal;
            snell_n = 1.0 / ir;
        }
        bool isRefract = refract(r_in.direction(), outward_normal, snell_n,
                                 refracted); //计算折射光线向量
        cosine = isOut ? dot(refracted, rec.normal)
                       : -(dot(r_in.direction(), rec.normal) /
                           r_in.direction().length());
        if (isRefract)
            // scattered = ray(rec.p, refracted);
            reflect_prob = reflectance(cosine, ir);
        else
            reflect_prob = 1.0;
        if (get_random() < reflect_prob)
            srec.specular_ray = ray(rec.p, reflected, r_in.time());
        else
            srec.specular_ray = ray(rec.p, refracted, r_in.time());
        return true;
    }

public:
    double ir; // Index of Refraction

private:
    static double reflectance(double cosine, double ref_idx) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};

class diffuse_light : public material {
public:
    diffuse_light(shared_ptr<texture> a) : emit(a) {}
    diffuse_light(color c) : emit(make_shared<constant_texture>(c)) {}

    virtual color emitted(const ray &r_in, const hit_record &rec, double u,
                          double v, const point3 &p) const override {
        if (!rec.front_face)
            return color(0, 0, 0);
        return emit->value(u, v, p);
    }

public:
    shared_ptr<texture> emit;
};

class isotropic : public material {
public:
    isotropic(shared_ptr<texture> a) : albedo(a) {}

    virtual bool scatter(const ray &r_in, const hit_record &rec,
                         vec3 &attenuation, ray &scattered, double &pdf) const {
        scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

public:
    shared_ptr<texture> albedo;
};

#endif // RAYTRACE_MATERIAL_HPP
