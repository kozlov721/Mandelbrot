// Author: Martin Kozlovsky
// Github: https://github.com/xkozlov1/MandelbrotZoom
//
//
// Have fun!

#include <string>
#include <iostream>

#include "../include/Window.h"

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
    int height = width * 2 / 3;
    // if (args == 3) {
    //     try {
    //         width = std::stoi(argv[1]);
    //     } catch (std::invalid_argument &e) {
    //         std::cout << "Invalid width" << std::endl;
    //         print_help();
    //         return INVALID_WIDTH;
    //     }
    // }    
    Window window(width, height);
    window.run();
    return SUCCESS;
}
