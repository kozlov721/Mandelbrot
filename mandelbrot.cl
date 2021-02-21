#define WIDTH (300 * 2)
#define HEIGHT (200 * 2)
#define N_ITERS 1000
#define K_COLOR 10.
#define A_COLOR (1. * (1. / K_COLOR))
#define B_COLOR (0.2357022604 * (1. / K_COLOR))
#define C_COLOR (0.58490406881 * (1. / K_COLOR))
#include "stdio.h"

typedef set_type long double;


int iterate(set_type c_r, set_type c_j, unsigned short n_iters) {
    set_type z_r = 0;
    set_type z_j = 0;
    set_type new_r;
    unsigned short i;
    for (i = 0; i < n_iters; ++i) {
        new_r = z_r * z_r - z_j * z_j;
        z_j = 2 * z_r * z_j;
        z_r = new_r;
        z_r += c_r;
        z_j += c_j;
        if (z_r * z_r + z_j * z_j >= 4)
            break;
    }
    return i;
}

__kernel void generate_set(__global sf::Uint8 *setArray, const set_type divider,
                  const set_type real_shift, const set_type imag_shift,
                  const int n_iters) {

    int i = get_global_id(0);
    int j = (i / 4) / WIDTH;
    int r = (i / 4) % WIDTH;
    set_type c_r = (((set_type) r / WIDTH - 0.5) * 3) / divider - real_shift;
    set_type c_j = (((set_type) j / HEIGHT - 0.5) * 2) / divider - imag_shift;
//    int iterations = iterate(c_r, c_j, n_iters);
    setArray[i] = 125; //std::floor(255 * ((1 - std::cos(A_COLOR * iters)) / 2));
    setArray[i + 1] = 125; //std::floor(255 * ((1 - std::cos(B_COLOR * iters)) / 2));
    setArray[i + 2] = 125; //std::floor(255 * ((1 - std::cos(C_COLOR * iters)) / 2));
}