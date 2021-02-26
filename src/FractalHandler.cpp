#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "../include/FractalHandler.h"

#define ITER_SPEED 20
#define WIDTH_8K (3840 * 2)
#define HEIGHT_8K (WIDTH_8K * 2 / 3)

FractalHandler::FractalHandler(Fractal *fractal, set_type initial_zoom,
                               set_type initial_real_shift,
                               set_type initial_imag_shift):
                           fractal(fractal), initial_zoom(initial_zoom) {
    zoom_value = initial_zoom;
    real_shift = initial_real_shift;
    imag_shift = initial_imag_shift;
}

FractalHandler::~FractalHandler() {
    delete fractal;
}

void FractalHandler::generate(int width, int height, uint8_t *pixels) {
    fractal->generate(zoom_value, width, height,
                      real_shift, imag_shift, max_iters, pixels);
}

void FractalHandler::adjust_max_iters() {
    max_iters = std::max(INITIAL_MAX_ITERS,
                         int(std::log(zoom_value) * ITER_SPEED) + max_iterations_shift);
}

void FractalHandler::save(const std::string &name) const {
    auto *pixels = new uint8_t[WIDTH_8K * HEIGHT_8K * 4];
    #pragma acc enter data create(pixels[0:WIDTH_8K * HEIGHT_8K * 4])
    fractal->generate(zoom_value, WIDTH_8K, HEIGHT_8K,
                      real_shift, imag_shift, max_iters, pixels);
    std::ofstream out(name, std::ios::binary | std::ios::out | std::ios::trunc);
    out << "P6\n" << WIDTH_8K << " " << HEIGHT_8K << "\n255\n";
    for (int i = 0; i < WIDTH_8K * HEIGHT_8K * 4; i += 4) {
        for (int j = 0; j < 3; ++j) {
            const char pixel = pixels[i + j];
            out.write(&pixel, 1);
        }
    }
    #pragma acc exit data delete(pixels[0:WIDTH_8K * HEIGHT_8K * 4])
    delete[] pixels;
}

void FractalHandler::change_iter_shift(int change) {
    max_iterations_shift += change;
}

bool FractalHandler::change_param(int param_index, bool fast, bool reverse) const {
    return fractal->change_param(param_index, fast, reverse ? -1 : 1);
}

void FractalHandler::zoom(set_type amount) {
    zoom_value *= amount;
}

void FractalHandler::move(set_type d_r, set_type d_i) {
    real_shift += d_r / zoom_value;
    imag_shift += d_i / zoom_value;
}

void FractalHandler::reset(bool reset_zoom, bool reset_iters, bool reset_params) {
    zoom_value = reset_zoom ? initial_zoom : zoom_value;
    max_iterations_shift = reset_iters ? INITIAL_ITER_SHIFT : max_iterations_shift;
    if (reset_params)
        fractal->reset_params();
}

set_type FractalHandler::get_zoom() const {
    return zoom_value;
}

std::string FractalHandler::generate_info_string() const {
    std::ostringstream stream;
    stream << std::scientific << std::setprecision(2) <<
        "Zoom: " << zoom_value << std::endl <<
        "Iterations: " << max_iters << std::endl <<
        fractal->get_params_info();
    return stream.str();
}

int FractalHandler::get_info_height() const {
    return fractal->get_num_params() + 2;
}

float FractalHandler::get_height_to_width_ratio() const {
    return fractal->height_to_width_ratio();
}
