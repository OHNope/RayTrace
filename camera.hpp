//
// Created by XBY on 2021/11/27.
//

#ifndef RAYTRACE_CAMERA_HPP
#define RAYTRACE_CAMERA_HPP

#include "./externalTools.hpp"
#include "./ray.hpp"
#include "./vec3.hpp"
#include <random>

using namespace std;

class camera {
public:
    // lower_left_corner(vec3(-2.0, -1.0, -1.0)),
    //          horizontal(vec3(4.0, 0.0, 0.0)), vertical(vec3(0.0, 2.0, 0.0)),
    //          origin(vec3(0.0, 0.0, 0.0))
    camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect,
           double aperture, double focus_dist, double t0 = 0, double t1 = 0) {
        time0 = t0;
        time1 = t1;
        double theta = degrees_to_radians(vfov);
        double h = tan(theta / 2);
        auto viewport_height = 2.0 * h;
        auto viewport_width = aspect * viewport_height;

        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        origin = lookfrom;
        horizontal = focus_dist * viewport_width * u;
        vertical = focus_dist * viewport_height * v;
        lower_left_corner =
            origin - horizontal / 2 - vertical / 2 - focus_dist * w;
        lens_radius = aperture / 2;
    };
    ray get_ray(float s, float t) {
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x() + v * rd.y();
        return ray(origin + offset,
                   lower_left_corner + s * horizontal + t * vertical - origin -
                       offset,
                   random_double(time0, time1));
    };

private:
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;
    double time0, time1; // shutter open/close times
    double lens_radius;
};
#endif // RAYTRACE_CAMERA_HPP
