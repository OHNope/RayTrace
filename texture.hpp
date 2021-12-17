//
// Created by XBY on 2021/12/17.
//

#ifndef RAYTRACE_TEXTURE_HPP
#define RAYTRACE_TEXTURE_HPP

class texture {
public:
    virtual vec3 value(double u, double v, const vec3 &p) const = 0;
};

class constant_texture : public texture {
public:
    constant_texture() {}
    constant_texture(vec3 c) : color(c) {}

    virtual vec3 value(double u, double v, const vec3 &p) const {
        return color;
    }

public:
    vec3 color;
};

class checker_texture : public texture {
public:
    checker_texture() {}
    checker_texture(shared_ptr<texture> t0, shared_ptr<texture> t1)
        : even(t0), odd(t1) {}

    virtual vec3 value(double u, double v, const vec3 &p) const {
        auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        if (sines < 0)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }

public:
    shared_ptr<texture> odd;
    shared_ptr<texture> even;
};

#endif // RAYTRACE_TEXTURE_HPP
