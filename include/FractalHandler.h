#pragma once

#define INITIAL_MAX_ITERS 30
#define INITIAL_ITER_SHIFT 0
#define INITIAL_REAL_SHIFT .0
#define INITIAL_IMAG_SHIFT .0
#define INITIAL_ZOOM 1.

#include <string>
#include "AbstractFractal.h"


class FractalHandler {
    int max_iterations_shift = INITIAL_ITER_SHIFT;
    int max_iters = INITIAL_MAX_ITERS;
    set_type zoom_value = INITIAL_ZOOM;
    set_type real_shift = INITIAL_REAL_SHIFT;
    set_type imag_shift = INITIAL_IMAG_SHIFT;

    Fractal *fractal;

public:
    FractalHandler(Fractal *fractal);
    void generate(int width, int height, uint8_t *pixels);
    void adjust_max_iters();
    void save(const std::string &name) const;
    void change_iter_shift(int change);
    bool change_param(int param_index, bool fast, bool reverse) const;
    std::string generate_info_string() const;
    void zoom(set_type amount);
    void move(set_type d_r, set_type d_i);
    void reset(bool reset_zoom, bool reset_iters, bool reset_params);

    set_type get_zoom() const;
};
