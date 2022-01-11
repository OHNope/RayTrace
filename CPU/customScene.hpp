//
// Created by XBY on 2021/12/17.
//

#ifndef RAYTRACE_CUSTOMSCENE_HPP
#define RAYTRACE_CUSTOMSCENE_HPP

#include "./material.hpp"
#include "./sphere.hpp"
#define STB_IMAGE_IMPLEMENTATION

#include "./box.hpp"
#include "./constant_medium.hpp"
#include "./lib/stb_image.h"
#include "./rect.hpp"

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
    auto test = static_cast<hittableList>(make_shared<BVHNode>(world, 0, 1));
    printf("BVH Done\n");
    return test;
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

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(vec3(0, -1000, 0), 1000,
                                    make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(vec3(0, 2, 0), 2,
                                    make_shared<lambertian>(pertext)));

    return objects;
}

hittableList earth() {
    int nx, ny, nn;
    unsigned char *texture_data = stbi_load("./earth.jpg", &nx, &ny, &nn, 0);

    auto earth_surface = make_shared<lambertian>(
        make_shared<image_texture>(texture_data, nx, ny));
    auto globe = make_shared<sphere>(vec3(0, 0, 0), 2, earth_surface);

    return hittableList(globe);
}

hittableList simple_light() {
    hittableList objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(vec3(0, -1000, 0), 1000,
                                    make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(vec3(0, 2, 0), 2,
                                    make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(
        make_shared<constant_texture>(vec3(4, 4, 4)));
    objects.add(make_shared<sphere>(vec3(0, 7, 0), 2, difflight));
    objects.add(make_shared<Rect<XY>>(3, 5, 1, 3, -2, difflight));

    return objects;
}

hittableList mutiply_angle() {
    hittableList world;

    auto material_ground = make_shared<lambertian>(
        make_shared<constant_texture>(color(0.8, 0.8, 0.0)));
    auto material_center = make_shared<lambertian>(
        make_shared<constant_texture>(color(0.1, 0.2, 0.5)));
    auto material_left = make_shared<dielectric>(1.5);
    auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

    world.add(
        make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(
        make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(
        make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.45, material_left));
    world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));
    return world;
}

hittableList cornell_smoke() {
    hittableList objects;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<Rect<YZ>>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<Rect<YZ>>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<flip_face>(
        make_shared<Rect<XZ>>(213, 343, 227, 332, 554, light)));
    objects.add(make_shared<Rect<XZ>>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<Rect<XZ>>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<Rect<XY>>(0, 555, 0, 555, 555, white));

    shared_ptr<hittable> box1 =
        make_shared<box>(vec3(0, 0, 0), vec3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));

    shared_ptr<hittable> box2 =
        make_shared<box>(vec3(0, 0, 0), vec3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));

    objects.add(make_shared<constant_medium>(
        box1, 0.01, make_shared<constant_texture>(vec3(0, 0, 0))));
    objects.add(make_shared<constant_medium>(
        box2, 0.01, make_shared<constant_texture>(vec3(1, 1, 1))));

    return objects;
}

hittableList final_scene() {
    hittableList boxes1;
    auto ground = make_shared<lambertian>(
        make_shared<constant_texture>(vec3(0.48, 0.83, 0.53)));

    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1, 101);
            auto z1 = z0 + w;

            boxes1.add(
                make_shared<box>(vec3(x0, y0, z0), vec3(x1, y1, z1), ground));
        }
    }

    hittableList objects;

    objects.add(make_shared<BVHNode>(boxes1, 0, 1));

    auto light = make_shared<diffuse_light>(
        make_shared<constant_texture>(vec3(7, 7, 7)));
    objects.add(make_shared<flip_face>(
        make_shared<Rect<XZ>>(123, 423, 147, 412, 554, light)));

    auto center1 = vec3(400, 400, 200);
    auto center2 = center1 + vec3(30, 0, 0);
    auto moving_sphere_material = make_shared<lambertian>(
        make_shared<constant_texture>(vec3(0.7, 0.3, 0.1)));
    objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50,
                                           moving_sphere_material));

    objects.add(make_shared<sphere>(vec3(260, 150, 45), 50,
                                    make_shared<dielectric>(1.5)));
    objects.add(make_shared<sphere>(
        vec3(0, 150, 145), 50, make_shared<metal>(vec3(0.8, 0.8, 0.9), 10.0)));

    auto boundary = make_shared<sphere>(vec3(360, 150, 145), 70,
                                        make_shared<dielectric>(1.5));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(
        boundary, 0.2, make_shared<constant_texture>(vec3(0.2, 0.4, 0.9))));
    boundary =
        make_shared<sphere>(vec3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    objects.add(make_shared<constant_medium>(
        boundary, .0001, make_shared<constant_texture>(vec3(1, 1, 1))));

    int nx, ny, nn;
    auto tex_data = stbi_load("earth.jpg", &nx, &ny, &nn, 0);
    auto emat =
        make_shared<lambertian>(make_shared<image_texture>(tex_data, nx, ny));
    objects.add(make_shared<sphere>(vec3(400, 200, 400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.1);
    objects.add(make_shared<sphere>(vec3(220, 280, 300), 80,
                                    make_shared<lambertian>(pertext)));

    hittableList boxes2;
    auto white = make_shared<lambertian>(
        make_shared<constant_texture>(vec3(0.73, 0.73, 0.73)));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(vec3::random(0, 165), 10, white));
    }

    objects.add(make_shared<translate>(
        make_shared<rotate_y>(make_shared<BVHNode>(boxes2, 0.0, 1.0), 15),
        vec3(-100, 270, 395)));

    return objects;
}

hittableList cornell_box() {
    hittableList objects;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<Rect<YZ>>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<Rect<YZ>>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<flip_face>(
        make_shared<Rect<XZ>>(213, 343, 227, 332, 554, light)));
    objects.add(make_shared<Rect<XZ>>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<Rect<XZ>>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<Rect<XY>>(0, 555, 0, 555, 555, white));

    shared_ptr<hittable> box1 =
        make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    objects.add(box1);

    shared_ptr<hittable> box2 =
        make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));
    objects.add(box2);

    return objects;
}
hittableList test_cornell_box() {
    hittableList objects;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<Rect<YZ>>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<Rect<YZ>>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<flip_face>(
        make_shared<Rect<XZ>>(213, 343, 227, 332, 554, light)));
    objects.add(make_shared<Rect<XZ>>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<Rect<XZ>>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<Rect<XY>>(0, 555, 0, 555, 555, white));

    shared_ptr<material> aluminum =
        make_shared<lambertian>(color(0.8, 0.85, 0.88));
    shared_ptr<hittable> box1 =
        make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), aluminum);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    objects.add(box1);

    auto glass = make_shared<dielectric>(1.5);
    objects.add(make_shared<sphere>(point3(190, 90, 190), 90, glass));

    return objects;
}

#endif // RAYTRACE_CUSTOMSCENE_HPP
