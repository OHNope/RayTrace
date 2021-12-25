#include "./BVH.hpp"
#include "./camera.hpp"
#include "./customScene.hpp"

#include "ncurses.h"
#include "omp.h"
#include <cfloat>
#include <fstream>
#include <istream>
#include <string>

using namespace std;

color ray_color(const ray &r, const vec3 &background, const hittable &world,
                int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0, 0, 0);
    if (!world.hit(r, 0.001, FLT_MAX, rec)) {
        return background;
    }
    ray scattered;
    vec3 attenuation;
    vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;
    return emitted +
           attenuation * ray_color(scattered, background, world, depth - 1);
}

void write_color(vector<vector<int>> &out, color pixel_color, int position,
                 int SPP) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Replace NaN components with zero. See explanation in Ray Tracing: The
    // Rest of Your Life.
    if (r != r)
        r = 0.0;
    if (g != g)
        g = 0.0;
    if (b != b)
        b = 0.0;

    // Divide the color by the number of samples and gamma-correct for
    // gamma=2.0.
    auto scale = 1.0 / SPP;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    // Write the translated [0,255] value of each color component.
    out[position][0] = static_cast<int>(256 * clamp(r, 0.0, 0.999));
    out[position][1] = static_cast<int>(256 * clamp(g, 0.0, 0.999));
    out[position][2] = static_cast<int>(256 * clamp(b, 0.0, 0.999));
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
    const int Image_Width = 800;
    const int Image_Height = static_cast<int>(Image_Width / aspect_ratio);
    /*const auto aspect_ratio = 2.0 / 1.0;
    const int Image_Width = 400;
    const int Image_Height = static_cast<int>(Image_Width / aspect_ratio);*/
    const int SPP = 200;
    const int max_depth = 10;
    // World
    auto world = final_scene();
    const vec3 background(0, 0, 0);
    // Camera
    vec3 lookfrom = point3(478, 278, -600);
    vec3 lookat = point3(278, 278, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    auto vfov = 40.0;

    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture,
               dist_to_focus, 0.0, 1.0);
    // construct image source
    vector<vector<int>> Image(Image_Height * Image_Width);
    for (int i = 0; i < Image.size(); i++)
        Image[i] = move(vector<int>(3));
    // Render
    int y;
    omp_set_dynamic(numProcs);
    omp_set_num_threads(numProcs);
#pragma omp parallel for private(y)
    for (y = Image_Height - 1; y >= 0; --y) {
        // std::cerr << "\rScanlines remaining: " << y << ' ' << std::flush;
        for (int x = 0; x < Image_Width; ++x) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < SPP; ++s) {
                auto u = (x + random_double()) / (Image_Width - 1);
                auto v = (y + random_double()) / (Image_Height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, background, world, max_depth);
            }
            write_color(Image, pixel_color, y * Image_Width + x, SPP);
            // ouput info
            omp_set_lock(&omp_lock);
            move(omp_get_thread_num() + 1, 2);
            printw("Thread%d:Pixel(%d, %d)  \t\0", omp_get_thread_num(), x, y);
            refresh();
            omp_unset_lock(&omp_lock);
        }
    }
    endwin();
    printf("calculation done!\n");
    ofstream os("test.png");
    os << "P3\n" << Image_Width << " " << Image_Height << "\n255\n";

    for (int y = Image_Height - 1; y >= 0; y--)
        for (int x = 0; x < Image_Width; x++)
            os << Image[y * Image_Width + x][0] << " "
               << Image[y * Image_Width + x][1] << " "
               << Image[y * Image_Width + x][2] << '\n';
    os.close();
    double end = omp_get_wtime();
    printf("time used:%lf\nMax num of threads:%d\nsuccessfully saved!\n",
           end - start, numProcs);
}