#pragma once

#include "FractalHandler.h"

FractalHandler build_mandelbrot_set();
FractalHandler build_julia_set(set_type c_r, set_type c_j);
FractalHandler build_burning_ship();
