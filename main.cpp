// Author: Martin Kozlovsky
// Github: https://github.com/xkozlov1/Mandelbrot
//
// Have fun!

#include "classes.h"


#define DEFAULT_WIDTH (300 * 4)
#define DEFAULT_HEIGHT (200 * 4)


int main(int args, const char **argv) {
    int width = DEFAULT_WIDTH;
    int height = DEFAULT_HEIGHT;

    if (args == 3) {
        width = std::stoi(argv[1]);
        height = std::stoi(argv[2]);
    } else if (args == 2) {
        width = std::stoi(argv[1]);
        height = width * 2 / 3;
    }

    Visualizer app(width, height);
    app.run();
    return 0;
}
