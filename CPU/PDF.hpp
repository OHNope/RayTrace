//
// Created by XBY on 2021/12/26.
//

#ifndef RAYTRACE_PDF_HPP
#define RAYTRACE_PDF_HPP

#include "./ONB.hpp"
#include "./externalTools.hpp"
#include "./vec3.hpp"

class pdf {
public:
    virtual ~pdf() {}

    virtual double value(const vec3 &direction) const = 0;
    virtual vec3 generate() const = 0;
};

class cosine_pdf : public pdf {
public:
    cosine_pdf(const vec3 &w) { uvw.build_from_w(w); }

    virtual double value(const vec3 &direction) const override {
        auto cosine = dot(unit_vector(direction), uvw.w());
        return (cosine <= 0) ? 0 : cosine / M_PI;
    }

    virtual vec3 generate() const override {
        return uvw.local(random_cosine_direction());
    }

public:
    onb uvw;
};

#endif // RAYTRACE_PDF_HPP
