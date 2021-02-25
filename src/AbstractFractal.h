#pragma once
#include <cstdint>

#ifdef GPU
typedef double set_type;
#else
typedef long double set_type;
#endif

class Fractal {
    void *additional_params;
protected:
    virtual int iterate(const set_type &c_r, const set_type &c_j) const = 0;
    virtual void fill_color(int index, int iterations) = 0;
public:
    Fractal();
    explicit Fractal(void* params);
    void generate(set_type zoom, int width, int height,
                  int width_shift, int height_shift,
                  set_type real_axis_shift, set_type imag_axis_shift,
                  uint8_t *pixels);
};
