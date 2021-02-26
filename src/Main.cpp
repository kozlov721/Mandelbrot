// Author: Martin Kozlovsky
// Github: https://github.com/xkozlov1/MandelbrotZoom
//
//
// Have fun!
#include "../include/Window.h"
#include "../include/MandelbrotSet.h"
#include "../include/JuliaSet.h"
#include "../include/BurningShip.h"

#define DEFAULT_WIDTH (300 * 4)


int main(int args, const char **argv) {
    int width = DEFAULT_WIDTH;
    if (args == 2) {
        width = std::stoi(argv[1]);
    }

    BurningShip fractal;
    Window window(width, width * 2 / 3, &fractal);
//    JuliaSet fractal(0.285, 0.01);
    window.run();
    return 0;
}
