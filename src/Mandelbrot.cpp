#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "../include/Mandelbrot.h"

#define ITER_SPEED 20
#define WIDTH_8K (3840 * 2)
#define HEIGHT_8K (WIDTH_8K * 2 / 3)


void Mandelbrot::generate(int width, int height, uint8_t *pixels) const {
    int len = width * height * 4;
    _Pragma("acc data present(pixels[0:len])")
    {
        _Pragma("acc parallel loop")
        for (int i = 0; i < len; i += 4) {
            int x = (i / 4) % width;
            int y = (i / 4) / width;
            double c_r = (3 * x - width) / (width * zoom_value) - real_shift;
            double c_j = (2 * y - height) / (height * zoom_value) - imag_shift;
            
            double z_r = 0.;
            double z_j = 0.;
            double new_r;
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
            
            pixels[i] = short(255 * ((1 - std::cos(r * float(iters) / k)) / 2));
            pixels[i + 1] = short(255 * ((1 - std::cos(g * float(iters) / k)) / 2));
            pixels[i + 2] = short(255 * ((1 - std::cos(b * float(iters) / k)) / 2));

        }

        // _Pragma("acc parallel loop")
        // for (int i = 4; i < len - 1; i += 8) {
        //     for (int j = 0; j < 3; ++j) {
        //         pixels[i + j] = (pixels[i + j - 4] + pixels[i + j + 4]) / 2;
        //     }
            
            
        //     // pixels[i] = short(255 * ((1 - std::cos(r * float(iters) / k)) / 2));
        //     // pixels[i + 1] = short(255 * ((1 - std::cos(g * float(iters) / k)) / 2));
        //     // pixels[i + 2] = short(255 * ((1 - std::cos(b * float(iters) / k)) / 2));

        // }

    _Pragma("acc update self(pixels[0:len])")
    }
}

void Mandelbrot::adjust_max_iters() {
    max_iters = std::max(INITIAL_MAX_ITERS,
                         int(std::log(zoom_value) * ITER_SPEED) + max_iterations_shift);
}

void Mandelbrot::save(const std::string &name) const {
    // auto *pixels = new uint8_t[WIDTH_8K * HEIGHT_8K * 4];
    // #pragma acc enter data create(pixels[0:WIDTH_8K * HEIGHT_8K * 4])
    // generate(zoom_value, WIDTH_8K, HEIGHT_8K,
    //          real_shift, imag_shift, max_iters, pixels);
    // std::ofstream out(name, std::ios::binary | std::ios::out | std::ios::trunc);
    // out << "P6\n" << WIDTH_8K << " " << HEIGHT_8K << "\n255\n";
    // for (int i = 0; i < WIDTH_8K * HEIGHT_8K * 4; i += 4) {
    //     for (int j = 0; j < 3; ++j) {
    //         const char pixel = pixels[i + j];
    //         out.write(&pixel, 1);
    //     }
    // }
    // #pragma acc exit data delete(pixels[0:WIDTH_8K * HEIGHT_8K * 4])
    // delete[] pixels;
}

void Mandelbrot::change_iter_shift(int change) {
    max_iterations_shift += change;
}

bool Mandelbrot::change_param(int param_index, bool shift, bool ctrl) {
    if (param_index > 3 || param_index < 0)
        return false;
    double speed = shift ? 5 : 1;
    double reverse = ctrl ? -1 : 1;

    if (param_index == 0)
        k += speed * reverse * 0.1;
    else if (param_index == 1)
        r += speed * reverse * 0.1;
    else if (param_index == 2)
        g += speed * reverse * 0.1;
    else if (param_index == 3)
        b += speed * reverse * 0.1;
    return true;
    
}

void Mandelbrot::zoom(double amount) {
    zoom_value *= amount;
}

void Mandelbrot::move(double d_r, double d_i) {
    real_shift += d_r / zoom_value;
    imag_shift += d_i / zoom_value;
}

void Mandelbrot::reset(bool reset_zoom, bool reset_iters, bool reset_params) {
    zoom_value = reset_zoom ? initial_zoom : zoom_value;
    max_iterations_shift = reset_iters ? INITIAL_ITER_SHIFT : max_iterations_shift;
    if (reset_params) {
        k = INITIAL_K;
        r = INITIAL_R;
        g = INITIAL_G;
        b = INITIAL_B;
    }
}

double Mandelbrot::get_zoom() const {
    return zoom_value;
}

std::string Mandelbrot::generate_info_string() const {
    std::ostringstream stream;
    stream << std::scientific << std::setprecision(2) <<
        "Zoom: " << zoom_value << std::endl <<
        "Iterations: " << max_iters << std::endl <<
        "K: " << k << std::endl <<
        "R: " << r << std::endl <<
        "G: " << g << std::endl <<
        "B: " << b << std::endl;
    return stream.str();
}

