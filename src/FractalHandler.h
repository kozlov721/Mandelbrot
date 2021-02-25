#pragma once

#define INITIAL_MAX_ITERS 30
#define INITIAL_ITER_SHIFT 0
#define INITIAL_WIDTH_SHIFT 0
#define INITIAL_HEIGHT_SHIFT 0
#define INITIAL_REAL_SHIFT .0
#define INITIAL_IMAG_SHIFT .0
#define INITIAL_ZOOM 1.

#include <string>
#include "AbstractFractal.h"


class FractalHandler {
    int max_iterations_shift = INITIAL_ITER_SHIFT;
    int max_iters = INITIAL_MAX_ITERS;
    int width_shift = INITIAL_WIDTH_SHIFT;
    int height_shift = INITIAL_HEIGHT_SHIFT;
    set_type zoom_value = INITIAL_ZOOM;
    set_type real_shift = INITIAL_REAL_SHIFT;
    set_type imag_shift = INITIAL_IMAG_SHIFT;

    Fractal *fractal;
    const int width;
    const int height;
    const int len;

public:
    FractalHandler(int width, int height, Fractal *fractal);
    void generate(uint8_t *pixels);
    void adjust_max_iters();
    void save(const std::string &name) const;
    void change_iter_shift(int change);
    void zoom(set_type amount);
    void move(set_type d_r, set_type d_i);
    void reset(bool reset_zoom, bool reset_iters);

    set_type get_zoom() const;
    int get_max_iters() const;
};
