#include "./BVH.hpp"
#include "./camera.hpp"
#include "./hittable.hpp"
#include "./material.hpp"
#include "./sphere.hpp"

#include "omp.h"
#include <cfloat>
#include <fstream>
#include <istream>
#include <string>

using namespace std;

color ray_color(const ray &r, const hittable &world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0, 0, 0);

    if (world.hit(r, 0.001, FLT_MAX, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return color(0, 0, 0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}
/*
hittableList random_scene() {
    hittableList world;

    auto ground_material = make_shared<lambertain>(color(0.5, 0.5, 0.5));
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
                    sphere_material = make_shared<lambertain>(albedo);
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

    auto material2 = make_shared<lambertain>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}
*/

hittableList random_scene() {
    hittableList world;
    world.add(make_shared<sphere>(
        vec3(0, -1000, 0), 1000, make_shared<lambertain>(vec3(0.5, 0.5, 0.5))));

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
                        1.0, 0.2, make_shared<lambertain>(albedo)));
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
        vec3(-4, 1, 0), 1.0, make_shared<lambertain>(vec3(0.4, 0.2, 0.1))));
    world.add(make_shared<sphere>(
        vec3(4, 1, 0), 1.0, make_shared<metal>(vec3(0.7, 0.6, 0.5), 0.0)));

    return world;
}
int main() {
    vec3 RED(1, 0.5, 0.5);
    double start = omp_get_wtime(); //获取起始时间
    int numProcs = omp_get_num_procs();
    // Image

    /*const auto aspect_ratio = 16.0 / 9.0;
    const int Image_Width = 1200;
    const int Image_Height = static_cast<int>(Image_Width / aspect_ratio);
*/ const auto aspect_ratio = 2.0 / 1.0;
    const int Image_Width = 200;
    const int Image_Height = static_cast<int>(Image_Width / aspect_ratio);
    const int SPP = 100;
    const int max_depth = 8;

    // World

    auto world = random_scene();
    /*hittableList world;
    auto material_ground = make_shared<lambertain>(vec3(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertain>(vec3(0.1, 0.2, 0.5));
    auto material_left = make_shared<dielectric>(1.5);
    auto material_right = make_shared<metal>(vec3(0.8, 0.6, 0.2), 0.0);

    world.add(
        make_shared<sphere>(vec3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(vec3(0.0, 0.0, -1.0), 0.5, material_center));
    world.add(make_shared<sphere>(vec3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(make_shared<sphere>(vec3(-1.0, 0.0, -1.0), -0.45, material_left));
    world.add(make_shared<sphere>(vec3(1.0, 0.0, -1.0), 0.5, material_right));
    world.add(make_shared<Triangle>(vec3(-0.5, -0.5, 0), vec3(0.0, 0.5, 0),
                                    vec3(0.5, -0.5, 0),
                                    make_shared<lambertain>(RED)));*/

    // Camera

    vec3 lookfrom(13, 2, 3);
    vec3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus,
               0.0, 1.0);

    // construct image source
    vector<vector<int>> Image(Image_Height * Image_Width);
    for (int i = 0; i < Image.size(); i++)
        Image[i] = move(vector<int>(3));
    // Render
    int y;
    omp_set_dynamic(1);
    omp_set_num_threads(50);
#pragma omp parallel for private(y)
    for (y = Image_Height - 1; y >= 0; --y) {
        // std::cerr << "\rScanlines remaining: " << y << ' ' << std::flush;
        for (int x = 0; x < Image_Width; ++x) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < SPP; ++s) {
                auto u = (x + random_double()) / (Image_Width - 1);
                auto v = (y + random_double()) / (Image_Height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            pixel_color /= SPP;
            Image[y * Image_Width + x][0] =
                static_cast<int>(255.999 * pixel_color.x());
            Image[y * Image_Width + x][1] =
                static_cast<int>(255.999 * pixel_color.y());
            Image[y * Image_Width + x][2] =
                static_cast<int>(255.999 * pixel_color.z());
        }
    }
    printf("calculation done!\n");
    ofstream os("test.png");
    os << "P3\n" << Image_Width << " " << Image_Height << "\n255\n";

    for (int i = Image.size() - 1; i >= 0; i--)
        os << Image[i][0] << " " << Image[i][1] << " " << Image[i][2] << '\n';
    os.close();
    double end = omp_get_wtime();
    printf("time used:%lf\nMax num of threads:%d\nsuccessfully saved!\n",
           end - start, numProcs);
}