#pragma once

#include "AbstractFractal.h"


class JuliaSet : public Fractal {
    static inline int iterate(set_type z_r, set_type z_j,
                              const set_type &c_r, const set_type &c_j,
                              int max_iters);
    inline void fill_with_color(uint8_t *begin, int iterations, int index) const;
public:
    JuliaSet(set_type c_r, set_type c_j);
    ~JuliaSet() override;
    void generate(set_type zoom, int width, int height,
                  set_type real_axis_shift, set_type imag_axis_shift,
                  int max_iters, uint8_t *pixels) const override;
    std::string get_params_info() const override;
    int get_num_params() const override;
    void change_param(int param_index, bool fast, int direction) override;
};
