#pragma once
#include <cstdint>

typedef double set_type;

class Fractal {
protected:
    set_type *additional_parameters = nullptr;
public:
    virtual void generate(set_type zoom, int width, int height,
                  set_type real_axis_shift, set_type imag_axis_shift,
                  int max_iters, uint8_t *pixels) const = 0;
    virtual ~Fractal() = default;
    virtual const set_type *get_params(int *n_params) const = 0;
    virtual void set_param(set_type param, int param_index) = 0;
};
