#include <cmath>

#include "../include/BurningShip.h"

#define INITIAL_K 10.f
#define INITIAL_R 1.f
#define INITIAL_G 0.2357022604f
#define INITIAL_B 0.58490406881f

BurningShip::BurningShip() {
    params = new set_type[4];
#pragma acc enter data copyin(this) create(params[0:4])
    params[0] = INITIAL_K;
    params[1] = INITIAL_R;
    params[2] = INITIAL_G;
    params[3] = INITIAL_B;
#pragma acc update device(this->params[0:4])
}

BurningShip::~BurningShip() {
#pragma acc exit data delete(this->params[0:4])
    delete[] params;
}

inline void BurningShip::fill_with_color(uint8_t *pixels, int iters, int index) const {
    pixels[index] = short(255 * ((1 - std::cos(params[1] * float(iters) / params[0])) / 2));
    pixels[index + 1] = short(255 * ((1 - std::cos(params[2] * float(iters) / params[0])) / 2));
    pixels[index + 2] = short(255 * ((1 - std::cos(params[3] * float(iters) / params[0])) / 2));
}

inline int BurningShip::iterate(const set_type &c_r, const set_type &c_j, const int max_iters) {
    set_type z_r = 0.;
    set_type z_j = 0.;
    set_type new_r;
    int i;
    for (i = 0; i < max_iters; ++i) {
        z_r = std::abs(z_r);
        z_j = std::abs(z_j);
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

void BurningShip::generate(set_type zoom, int width, int height,
                             set_type real_axis_shift, set_type imag_axis_shift,
                             int max_iters, uint8_t *pixels) const {
    int len = width * height * 4;
#pragma acc data present(pixels[0:len], this->params[0:4])
#pragma acc update device(this->params[0:4])
    {
#pragma acc parallel loop
        for (int i = 0; i < len; i += 4) {
            int x = (i / 4) % width;
            int y = (i / 4) / width;
            set_type c_r = (3 * x - 2 * width) / (width * zoom) - real_axis_shift;
            set_type c_j = (2 * y - height) / (height * zoom) - imag_axis_shift;
            int iters = iterate(c_r, c_j, max_iters);
            fill_with_color(pixels, iters, i);
        }
#pragma acc update self(pixels[0:len])
    }
}

std::string BurningShip::get_params_info() const {
    return "K: " + std::to_string(params[0]) + "\n"
           + "R: " + std::to_string(params[1]) + "\n"
           + "G: " + std::to_string(params[2]) + "\n"
           + "B: " + std::to_string(params[3]) + "\n";
}

int BurningShip::get_num_params() const {
    return 4;
}

void BurningShip::change_param(int param_index, bool fast, int direction) {
    if (param_index > 3) return;
    set_type speed = fast ? 10. : 1.;
    set_type change = param_index == 0 ? 0.5 : 0.05;
    params[param_index] += direction * speed * change;
}
