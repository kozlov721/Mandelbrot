#include <cmath>

#include "Fractals.h"

#define INITIAL_K 10.f
#define INITIAL_R 1.f
#define INITIAL_G 0.2357022604f
#define INITIAL_B 0.58490406881f

#define GENERATE_WITH \
int len = width * height * 4; \
#pragma acc data present(pixels[0:len]) \
{ \
#pragma acc parallel loop \
for (int i = 0; i < width * height * 4; i += 4) { \
int j = (i / 4) / width; \
int r = (i / 4) % width; \
set_type c_r = (((set_type) r / width - 0.5) * 3) / zoom - real_axis_shift; \
set_type c_j = (((set_type) j / height - 0.5) * 2) / zoom - imag_axis_shift;\
int iters = \
//int iters = iterate(c_r, c_j, max_iters); \
#define END_GENERATING \
fill_with_color(pixels, iters, i); \
} \
#pragma acc update self(pixels[0:len]) \
} \





inline int MandelbrotSet::iterate(const set_type &c_r, const set_type &c_j, const int max_iters) {
    set_type z_r = 0.;
    set_type z_j = 0.;
    set_type new_r;
    int i;
    for (i = 0; i < max_iters; ++i) {
        new_r = z_r * z_r - z_j * z_j;
        z_j = 2 * z_r * z_j;
        z_r = new_r;
        z_r += c_r;
        z_j += c_j;
        if (z_r * z_r + z_j * z_j >= 4.)
            break;
    }
    return i;
}

inline void fill_with_color(uint8_t *pixels, int iters, int index) {
    pixels[index] = short(255 * ((1 - std::cos(INITIAL_R * float(iters) / INITIAL_K)) / 2));
    pixels[index + 1] = short(255 * ((1 - std::cos(INITIAL_G * float(iters) / INITIAL_K)) / 2));
    pixels[index + 2] = short(255 * ((1 - std::cos(INITIAL_B * float(iters) / INITIAL_K)) / 2));
}

void MandelbrotSet::generate(set_type zoom, int width, int height,
                       set_type real_axis_shift, set_type imag_axis_shift,
                       int max_iters, uint8_t *pixels) const {
    #pragma acc data present(pixels[0:width * height * 4])
    {
        #pragma acc parallel loop
        for (int i = 0; i < width * height * 4; i += 4) {
            int x = (i / 4) % width;
            int y = (i / 4) / width;
            set_type c_r = (3 * x - 2 * width) / (width * zoom) - real_axis_shift;
            set_type c_j = (2 * y - height) / (height * zoom) - imag_axis_shift;
            int iters = iterate(c_r, c_j, max_iters);
            fill_with_color(pixels, iters, i);
        }
    #pragma acc update self(pixels[0:width * height * 4])
    }
}

JuliaSet::JuliaSet(set_type c_r, set_type c_j) {
    additional_parameters = new set_type[2];
#pragma acc enter data copyin(this) create(additional_parameters[0:2])
    additional_parameters[0] = c_r;
    additional_parameters[1] = c_j;
#pragma acc update device(this->additional_parameters[0:2])

}

JuliaSet::~JuliaSet() {
    delete[] additional_parameters;
}

int JuliaSet::iterate(set_type &z_r, set_type &z_j, const int &max_iters) const {
#pragma acc data present(this->additional_parameters[0:2])
    {
        set_type new_r;
        int i;
        for (i = 0; i < max_iters; ++i) {
            new_r = z_r * z_r - z_j * z_j;
            z_j = 2 * z_r * z_j;
            z_r = new_r;
            z_r += ((set_type *) additional_parameters)[0];
            z_j += ((set_type *) additional_parameters)[1];
            if (z_r * z_r + z_j * z_j >= 4.)
                break;
        }
        return i;
    }
}

void JuliaSet::generate(set_type zoom, int width, int height,
                        set_type real_axis_shift, set_type imag_axis_shift,
                        int max_iters, uint8_t *pixels) const {
    int len = width * height * 4;
    #pragma acc data present(pixels[0:len])
    {
        #pragma acc parallel loop
        for (int i = 0; i < width * height * 4; i += 4) {
            int j = (i / 4) / width;
            int r = (i / 4) % width;
            set_type c_r = (((set_type) r / width - 0.5) * 3) / zoom - real_axis_shift;
            set_type c_j = (((set_type) j / height - 0.5) * 2) / zoom - imag_axis_shift;
            int iters = iterate(c_r, c_j, max_iters);
//            pixels[i] = iters;
            fill_with_color(pixels, iters, i);
        }
    #pragma acc update self(pixels[0:len])
    }
}
