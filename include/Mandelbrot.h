#pragma once

#define INITIAL_MAX_ITERS 30
#define INITIAL_ITER_SHIFT 0
#define INITIAL_ZOOM 1.

#define INITIAL_K 10.
#define INITIAL_R 1.
#define INITIAL_G 0.2357022604
#define INITIAL_B 0.58490406881

#include <string>
#include <stdint.h>


class Mandelbrot {
    int max_iterations_shift = INITIAL_ITER_SHIFT;
    int max_iters = INITIAL_MAX_ITERS;
    const double initial_zoom = INITIAL_ZOOM;
    double zoom_value = INITIAL_ZOOM;
    double real_shift = 1.1;
    double imag_shift = 0;
    double k = INITIAL_K;
    double r = INITIAL_R;
    double g = INITIAL_G;
    double b = INITIAL_B;

public:
    void generate(int width, int height, uint8_t *pixels) const;
    void adjust_max_iters();
    void change_iter_shift(int change);
    bool change_param(int param_index, bool fast, bool reverse);
    std::string generate_info_string() const;
    void zoom(double amount);
    void move(double d_r, double d_i);
    void reset(bool reset_zoom, bool reset_iters, bool reset_params);

    std::string format_info() const;
};
