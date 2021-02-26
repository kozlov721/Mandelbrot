#pragma once
#include "EscapeTimeFractal.h"


class MandelbrotSet final : public EscapeTimeFractal {
    static int iterate(const set_type &c_r, const set_type &c_j, const int max_iters);
public:
    void generate(set_type zoom, int width, int height,
                  set_type real_axis_shift, set_type imag_axis_shift,
                  int max_iters, uint8_t *pixels) const override;
    float height_to_width_ratio() const override;
};


class BurningShip final : public EscapeTimeFractal {
    static int iterate(const set_type &c_r, const set_type &c_j, const int max_iters);
public:
    void generate(set_type zoom, int width, int height,
                  set_type real_axis_shift, set_type imag_axis_shift,
                  int max_iters, uint8_t *pixels) const override;
    float height_to_width_ratio() const override;
};


class JuliaSet final : public EscapeTimeFractal {
    static int iterate(set_type z_r, set_type z_j,
                       set_type c_r, set_type c_j,
                       int max_iters);
public:
    JuliaSet(set_type c_r, set_type c_j);
    void generate(set_type zoom, int width, int height,
                  set_type real_axis_shift, set_type imag_axis_shift,
                  int max_iters, uint8_t *pixels) const override;

    std::string get_params_info() const override;
    int get_num_params() const override;
    bool change_param(int param_index, bool fast, int direction) override;
    float height_to_width_ratio() const override;
};
