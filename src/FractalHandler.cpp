#include <cmath>
#include <fstream>

#include "FractalHandler.h"

#define ITER_SPEED 20
#define WIDTH_8K (3840 * 2)
#define HEIGHT_8K (WIDTH_8K * 2 / 3)

FractalHandler::FractalHandler(int width, int height, Fractal *fractal) :
        width(width), height(height), len(height * width * 4), fractal(fractal) {}

void FractalHandler::generate(uint8_t *pixels) {
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
}

void FractalHandler::change_iter_shift(int change) {
    max_iterations_shift += change;
}

void FractalHandler::zoom(set_type amount) {
    zoom_value *= amount;
}

void FractalHandler::move(set_type d_r, set_type d_i) {
    real_shift += d_r / zoom_value;
    imag_shift += d_i / zoom_value;
}

void FractalHandler::reset(bool reset_zoom, bool reset_iters) {
    zoom_value = reset_zoom ? INITIAL_ZOOM : zoom_value;
    max_iterations_shift = reset_iters ? INITIAL_ITER_SHIFT : max_iterations_shift;
}

set_type FractalHandler::get_zoom() const {
    return zoom_value;
}

int FractalHandler::get_max_iters() const {
    return max_iters;
}
