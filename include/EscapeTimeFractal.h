#pragma once

#include "AbstractFractal.h"
#include <cmath>


static inline void fill_with_color(uint8_t *begin, int iterations, const set_type *params) {
    begin[0] = short(255 * ((1 - std::cos(params[1] * float(iterations) / params[0])) / 2));
    begin[1] = short(255 * ((1 - std::cos(params[2] * float(iterations) / params[0])) / 2));
    begin[2] = short(255 * ((1 - std::cos(params[3] * float(iterations) / params[0])) / 2));
}

struct EscapeTimeFractal : public Fractal {
    EscapeTimeFractal();
    ~EscapeTimeFractal() override;
    std::string get_params_info() const override;
    int get_num_params() const override;
    bool change_param(int param_index, bool fast, int direction) override;
    void reset_params() override;
};
