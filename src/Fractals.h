#pragma once

#include "AbstractFractal.h"

static inline void fill_with_color(uint8_t *begin, int iterations, int index);

class MandelbrotSet : public Fractal {
    static inline int iterate(const set_type &c_r, const set_type &c_j, int max_iters);
public:
    void generate(set_type zoom, int width, int height,
                  set_type real_axis_shift, set_type imag_axis_shift,
                  int max_iters, uint8_t *pixels) const override;
};


class JuliaSet : public Fractal {
    inline int iterate(set_type &z_r, set_type &z_j, const int &max_iters) const;
public:
    JuliaSet(set_type c_r, set_type c_j);
    ~JuliaSet() override;
    void generate(set_type zoom, int width, int height,
                  set_type real_axis_shift, set_type imag_axis_shift,
                  int max_iters, uint8_t *pixels) const override;
};
