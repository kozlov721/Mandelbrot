// Author: Martin Kozlovsky
// Github: https://github.com/xkozlov1/MandelbrotZoom
//
//
// Have fun!

#include <string>
#include <iostream>

#include "../include/Window.h"
#include "../include/FractalBuilder.h"

#define DEFAULT_WIDTH (300 * 4)
#define SUCCESS 0
#define INVALID_WIDTH 1
#define INVALID_NAME 2

void print_help() {
    std::cout << "Invalid options. The correct usage is: WIDTH [mandelbrot|julia|ship]"
        << std::endl;
}

int main(int args, const char **argv) {

    int width = DEFAULT_WIDTH;
    if (args == 3) {
        try {
            width = std::stoi(argv[1]);
        } catch (std::invalid_argument &e) {
            std::cout << "Invalid width" << std::endl;
            print_help();
            return INVALID_WIDTH;
        }
    }
    std::string fractal_name = std::string(argv[2 - (width == DEFAULT_WIDTH)]);
    if (fractal_name == "mandelbrot") {
        FractalHandler handler = build_mandelbrot_set();
        Window window(width, &handler);
        window.run();
    } else if (fractal_name == "julia") {
        FractalHandler handler = build_julia_set(0.4, 0.1);
        Window window(width, &handler);
        window.run();
    }
    else if (fractal_name == "ship") {
        FractalHandler handler = build_burning_ship();
        Window window(width, &handler);
        window.run();
    }
    else {
        print_help();
        return INVALID_NAME;
    }
    return SUCCESS;
}
