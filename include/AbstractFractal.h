#pragma once
#include <cstdint>
#include <string>
#include <vector>

typedef double set_type;

class Fractal {
protected:
    set_type *params = nullptr;
public:
    virtual void generate(set_type zoom, int width, int height,
                  set_type real_axis_shift, set_type imag_axis_shift,
                  int max_iters, uint8_t *pixels) const = 0;
    virtual ~Fractal() = default;
    virtual std::string get_params_info() const = 0;
    virtual int get_num_params() const = 0;
    virtual bool change_param(int param_index, bool fast, int direction) = 0;
    virtual void reset_params() = 0;
};
