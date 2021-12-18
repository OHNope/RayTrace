//
// Created by XBY on 2021/12/17.
//

#ifndef RAYTRACE_CUSTOMSCENE_HPP
#define RAYTRACE_CUSTOMSCENE_HPP

#include "./material.hpp"
#include "./sphere.hpp"

/*
hittableList random_scene() {
    hittableList world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2,
                          b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(
                        make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(
                        make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(
                        make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}
*/

hittableList random_scene() {
    hittableList world;
    auto checker = make_shared<checker_texture>(
        make_shared<constant_texture>(vec3(0.2, 0.3, 0.1)),
        make_shared<constant_texture>(vec3(0.9, 0.9, 0.9)));

    world.add(make_shared<sphere>(vec3(0, -1000, 0), 1000,
                                  make_shared<lambertian>(checker)));

    int i = 1;
    for (int a = -5; a < 5; a++) {
        for (int b = -5; b < 5; b++) {
            auto choose_mat = random_double();
            vec3 center(a + 0.9 * random_double(), 0.2,
                        b + 0.9 * random_double());
            if ((center - vec3(4, .2, 0)).length() > 0.9) {
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = vec3::random() * vec3::random();
                    world.add(make_shared<moving_sphere>(
                        center, center + vec3(0, random_double(0, .5), 0), 0.0,
                        1.0, 0.2,
                        make_shared<lambertian>(
                            make_shared<constant_texture>(albedo))));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = vec3::random(.5, 1);
                    auto fuzz = random_double(0, .5);
                    world.add(make_shared<sphere>(
                        center, 0.2, make_shared<metal>(albedo, fuzz)));
                } else {
                    // glass
                    world.add(make_shared<sphere>(
                        center, 0.2, make_shared<dielectric>(1.5)));
                }
            }
        }
    }

    world.add(
        make_shared<sphere>(vec3(0, 1, 0), 1.0, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(
        vec3(-4, 1, 0), 1.0,
        make_shared<lambertian>(
            make_shared<constant_texture>(vec3(0.4, 0.2, 0.1)))));
    world.add(make_shared<sphere>(
        vec3(4, 1, 0), 1.0, make_shared<metal>(vec3(0.7, 0.6, 0.5), 0.0)));

    return static_cast<hittableList>(make_shared<BVHNode>(world, 0, 1));
}

hittableList init() {
    vec3 RED(1, 0.5, 0.5);
    hittableList world;
    auto material_ground = make_shared<lambertian>(
        make_shared<constant_texture>(vec3(0.8, 0.8, 0.0)));
    auto material_center = make_shared<lambertian>(
        make_shared<constant_texture>(vec3(0.1, 0.2, 0.5)));
    auto material_left = make_shared<dielectric>(1.5);
    auto material_right = make_shared<metal>(vec3(0.8, 0.6, 0.2), 0.0);

    world.add(
        make_shared<sphere>(vec3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(vec3(0.0, 0.0, -1.0), 0.5, material_center));
    world.add(make_shared<sphere>(vec3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(make_shared<sphere>(vec3(-1.0, 0.0, -1.0), -0.45, material_left));
    world.add(make_shared<sphere>(vec3(1.0, 0.0, -1.0), 0.5, material_right));
    world.add(make_shared<Triangle>(
        vec3(-0.5, -0.5, 0), vec3(0.0, 0.5, 0), vec3(0.5, -0.5, 0),
        make_shared<lambertian>(make_shared<constant_texture>(RED))));
    return world;
}

hittableList two_perlin_spheres() {
    hittableList objects;

    auto pertext = make_shared<noise_texture>();
    objects.add(make_shared<sphere>(vec3(0, -1000, 0), 1000,
                                    make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(vec3(0, 2, 0), 2,
                                    make_shared<lambertian>(pertext)));

    return objects;
}

#endif // RAYTRACE_CUSTOMSCENE_HPP