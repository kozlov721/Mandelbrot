#include <sstream>
#include <iomanip>

#include "../include/Fractals.h"

/**
 * I am genuinely sorry about this abomination, but it was necessary.
 * The compiler is unable to properly inline inherited and abstract functions
 * in order for openacc to work, so I had to do it this way to avoid duplication
 * of code.
 */
#define CREATE_GENERATE_METHOD(CLASS, ITERATE_FUNCTION, WIDTH_RATIO, HEIGHT_RATIO)                 \
void CLASS::generate(set_type zoom, int width, int height,                                         \
                     set_type real_axis_shift, set_type imag_axis_shift,                           \
                     int max_iters, uint8_t *pixels) const {                                       \
    int len = width * height * 4;                                                                  \
    int params_len = get_num_params();                                                             \
    _Pragma("acc data present(pixels[0:len], this->params[0:params_len])")                         \
    _Pragma("acc update device(this->params[0:params_len])")                                       \
    {                                                                                              \
        _Pragma("acc parallel loop")                                                               \
        for (int i = 0; i < len; i += 4) {                                                         \
            int x = (i / 4) % width;                                                               \
            int y = (i / 4) / width;                                                               \
            set_type c_r = (WIDTH_RATIO * x - width) / (width * zoom) - real_axis_shift;           \
            set_type c_j = (HEIGHT_RATIO * y - height) / (height * zoom) - imag_axis_shift;        \
            int iters = (ITERATE_FUNCTION);                                                        \
            fill_with_color(&pixels[i], iters, params);                                            \
        }                                                                                          \
    _Pragma("acc update self(pixels[0:len])")                                                      \
    }                                                                                              \
}                                                                                                  \


inline int MandelbrotSet::iterate(const set_type &c_r, const set_type &c_j, const int max_iters) {
    set_type z_r = 0.;
    set_type z_j = 0.;
    set_type new_r;
    int iters;
    for (iters = 0; iters < max_iters; ++iters) {
        new_r = z_r * z_r - z_j * z_j;
        z_j = 2 * z_r * z_j;
        z_r = new_r;
        z_r += c_r;
        z_j += c_j;
        if (z_r * z_r + z_j * z_j >= 4.)
            break;
    }
    return iters;
}

CREATE_GENERATE_METHOD(
        MandelbrotSet,
        iterate(c_r, c_j, max_iters),
        3,
        2
)

/*************************************************************/

inline int BurningShip::iterate(const set_type &c_r, const set_type &c_j, const int max_iters) {
    set_type z_r = 0.;
    set_type z_j = 0.;
    set_type new_r;
    int iters;
    for (iters = 0; iters < max_iters; ++iters) {
        z_r = std::abs(z_r);
        z_j = std::abs(z_j);
        new_r = z_r * z_r - z_j * z_j;
        z_j = 2 * z_r * z_j;
        z_r = new_r;
        z_r += c_r;
        z_j += c_j;
        if (z_r * z_r + z_j * z_j >= 4.)
            break;
    }
    return iters;
}

CREATE_GENERATE_METHOD(
        BurningShip,
        iterate(c_r, c_j, max_iters),
        3,
        2
)

/***********************************************************/

inline int JuliaSet::iterate(set_type z_r, set_type z_j,
                             set_type c_r, set_type c_j,
                             int max_iters) {
    set_type new_r;
    int i;
    for (i = 0; i < max_iters; ++i) {
        new_r = z_r * z_r - z_j * z_j;
        z_j = 2 * z_r * z_j;
        z_r = new_r;
        z_r += c_r;
        z_j += c_j;
        if (z_r * z_r + z_j * z_j >= 4.)
            break;
    }
    return i;
}

CREATE_GENERATE_METHOD(
        JuliaSet,
        iterate(c_r, c_j, params[4], params[5], max_iters),
        1,
        1
)

JuliaSet::JuliaSet(set_type r, set_type j) {
    params[4] = r;
    params[5] = j;
}

std::string JuliaSet::get_params_info() const {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(3) <<
        "C: " << params[4] << " + " << params[5] << "i" << std::endl;
    return stream.str();
}

int JuliaSet::get_num_params() const {
    return EscapeTimeFractal::get_num_params() + 2;
}

bool JuliaSet::change_param(int param_index, bool fast, int direction) {
    if (param_index > 6) return false;
    if (param_index < 4)
        return EscapeTimeFractal::change_param(param_index, fast, direction);
    set_type speed = fast ? 5. : 1.;
    params[param_index] += direction * speed * 0.01;
    return true;
}
