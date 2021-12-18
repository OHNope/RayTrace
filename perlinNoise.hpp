//
// Created by XBY on 2021/12/18.
//

#ifndef RAYTRACE_PERLINNOISE_HPP
#define RAYTRACE_PERLINNOISE_HPP

inline double trilinear_interp(double c[2][2][2], double u, double v,
                               double w) {
    auto accum = 0.0;
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            for (int k = 0; k < 2; k++)
                accum += (i * u + (1 - i) * (1 - u)) *
                         (j * v + (1 - j) * (1 - v)) *
                         (k * w + (1 - k) * (1 - w)) * c[i][j][k];

    return accum;
}

class perlin {
public:
    perlin() {
        ranfloat = new double[point_count];
        for (int i = 0; i < point_count; ++i) {
            ranfloat[i] = random_double();
        }

        perm_x = perlin_generate_perm();
        perm_y = perlin_generate_perm();
        perm_z = perlin_generate_perm();
    }

    ~perlin() {
        delete[] ranfloat;
        delete[] perm_x;
        delete[] perm_y;
        delete[] perm_z;
    }

    double noise(const vec3 &p) const {
        auto u = p.x() - floor(p.x());
        auto v = p.y() - floor(p.y());
        auto w = p.z() - floor(p.z());
        // hermite cube平滑差值 去除Mach bands
        u = u * u * (3 - 2 * u);
        v = v * v * (3 - 2 * v);
        w = w * w * (3 - 2 * w);

        int i = floor(p.x());
        int j = floor(p.y());
        int k = floor(p.z());
        double c[2][2][2];

        for (int di = 0; di < 2; di++)
            for (int dj = 0; dj < 2; dj++)
                for (int dk = 0; dk < 2; dk++)
                    c[di][dj][dk] = ranfloat[perm_x[(i + di) & 255] ^
                                             perm_y[(j + dj) & 255] ^
                                             perm_z[(k + dk) & 255]];

        return trilinear_interp(c, u, v, w);
    }

private:
    static const int point_count = 256;
    double *ranfloat;
    int *perm_x;
    int *perm_y;
    int *perm_z;

    static int *perlin_generate_perm() {
        auto p = new int[point_count];

        for (int i = 0; i < perlin::point_count; i++)
            p[i] = i;

        permute(p, point_count);

        return p;
    }

    static void permute(int *p, int n) {
        for (int i = n - 1; i > 0; i--) {
            int target = random_int(0, i);
            int tmp = p[i];
            p[i] = p[target];
            p[target] = tmp;
        }
    }
};

#endif // RAYTRACE_PERLINNOISE_HPP
