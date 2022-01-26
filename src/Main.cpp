// Author: Martin Kozlovsky
// Github: https://github.com/xkozlov1/MandelbrotZoom
//
//
// Have fun!

#include <string>
#include <iostream>

#include "../include/Window.h"

#define DEFAULT_WIDTH (300 * 4)


int main(int args, const char **argv) {

    int width = DEFAULT_WIDTH;

    if (args > 1) {
        try {
            width = std::stoi(argv[1]);
        } catch (std::invalid_argument &e) {
            std::cout << "Invalid width, must be a number." << std::endl;
            return 1;
        }

    }
    int height = width * 2 / 3;
    Window window(width, height);
    window.run();
    return 0;
}
