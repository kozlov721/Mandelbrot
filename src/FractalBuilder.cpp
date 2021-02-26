#include <memory>

#include "../include/FractalBuilder.h"
#include "../include/Fractals.h"



FractalHandler build_mandelbrot_set() {
    auto *fractal = new MandelbrotSet;
    FractalHandler handler(fractal, 1., 1.1, .0);
    return handler;
}

FractalHandler build_julia_set(set_type c_r, set_type c_j) {
    auto *fractal = new JuliaSet(c_r, c_j);
    FractalHandler handler(fractal, 1. / 1.3, .0, .0);
    return handler;
}

FractalHandler build_burning_ship() {
    auto *fractal = new BurningShip();
    FractalHandler handler(fractal, 1. / 1.1, .9, .5);
    return handler;
}
