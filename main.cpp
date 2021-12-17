#include "./BVH.hpp"
#include "./camera.hpp"
#include "./customScene.hpp"
#include "./hittable.hpp"
#include "./material.hpp"
#include "./sphere.hpp"

#include "ncurses.h"
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

int main() {
    // Log screen
    initscr();                         //初始化
    box(stdscr, ACS_VLINE, ACS_HLINE); //画边框
    curs_set(FALSE);                   /* 不显示光标 */
    //
    static omp_lock_t omp_lock;
    omp_init_lock(&omp_lock);
    double start = omp_get_wtime(); //获取起始时间
    int numProcs = omp_get_max_threads();
    // Image
    const auto aspect_ratio = 2 / 1;
    const int Image_Width = 400;
    const int Image_Height = static_cast<int>(Image_Width / aspect_ratio);
    /*const auto aspect_ratio = 2.0 / 1.0;
    const int Image_Width = 400;
    const int Image_Height = static_cast<int>(Image_Width / aspect_ratio);*/
    const int SPP = 30;
    const int max_depth = 5;
    // World
    auto world = random_scene();

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
    omp_set_num_threads(10);
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
            // ouput info
            omp_set_lock(&omp_lock);
            move(omp_get_thread_num() + 1, 2);
            printw("Thread%d:Pixel(%d, %d) \0", omp_get_thread_num(), x, y);
            refresh();
            omp_unset_lock(&omp_lock);
        }
    }
    endwin();
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