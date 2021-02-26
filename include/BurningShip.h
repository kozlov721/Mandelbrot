#pragma once

#include "AbstractFractal.h"


class BurningShip : public Fractal {
    static inline int iterate(const set_type &c_r, const set_type &c_j, int max_iters);
    inline void fill_with_color(uint8_t *begin, int iterations, int index) const;
public:
    BurningShip();
    ~BurningShip() override;
    void generate(set_type zoom, int width, int height,
                  set_type real_axis_shift, set_type imag_axis_shift,
                  int max_iters, uint8_t *pixels) const override;
    std::string get_params_info() const override;
    int get_num_params() const override;
    void change_param(int param_index, bool fast, int direction) override;
};
