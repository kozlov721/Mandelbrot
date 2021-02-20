#include <SFML/Graphics.hpp>
#include <omp.h>
#include <tuple>
#include <iostream>
#include <cmath>

#define WIDTH (300 * 3)
#define HEIGHT (200 * 3)
#define N_ITERS 500
#define K_COLOR 10.
#define A_COLOR (1. * (1. / K_COLOR))
#define B_COLOR (0.2357022604 * (1. / K_COLOR))
#define C_COLOR (0.58490406881 * (1. / K_COLOR))
inline void square_complex(long double &r, long double &j) {
    long double new_r = r * r - j * j;
    j = 2 * r * j;
    r = new_r;
}

inline void add_complex(long double &r1, long double &j1,
                        long double &r2, long double &j2) {
    r1 += r2;
    j1 += j2;
}

int iterate(long double c_r, long double c_j, int n_iters) {
    long double z_r = 0;
    long double z_j = 0;
    int i;
    for (i = 0; i < n_iters; ++i) {
        square_complex(z_r, z_j);
        add_complex(z_r, z_j, c_r, c_j);
        if (z_r * z_r + z_j * z_j >= 4)
            break;
    }
    return i;
}

void init_pixels(sf::Uint8 *setArray) {
    for (int i = 0; i < WIDTH * HEIGHT * 4; i += 4)
        setArray[i + 3] = 255;
}

inline void color(int iters, int max_iters, sf::Uint8 &r, sf::Uint8 &g, sf::Uint8 &b) {
    r = std::floor(255 * ((1 - std::cos(A_COLOR * iters)) / 2));
    g = std::floor(255 * ((1 - std::cos(B_COLOR * iters)) / 2));
    b = std::floor(255 * ((1 - std::cos(C_COLOR * iters)) / 2));
}

inline void continuous_color(int iters, int max_iters, sf::Uint8 &r, sf::Uint8 &g, sf::Uint8 &b) {
    r = std::floor(255 * ((1 - std::cos(A_COLOR * iters)) / 2));
    g = std::floor(255 * ((1 - std::cos(B_COLOR * iters)) / 2));
    b = std::floor(255 * ((1 - std::cos(C_COLOR * iters)) / 2));
}

void generate_set(sf::Uint8 *setArray, const long double divider,
                  const long double real_shift, const long double imag_shift,
                  const int n_iters) {
    #pragma omp parallel for num_threads(32)
    for (int i = 0; i < WIDTH * HEIGHT * 4; i += 4) {
        int j = (i / 4) / WIDTH;
        int r = (i / 4) % WIDTH;
        long double c_r = (((long double) r / WIDTH - 0.5) * 3) / divider - real_shift;
        long double c_j = (((long double) j / HEIGHT - 0.5) * 2) / divider - imag_shift;
        int iterations = iterate(c_r, c_j, n_iters);
        color(iterations, n_iters, setArray[i], setArray[i + 1], setArray[i + 2]);
//        std::cout << iterations << std::endl;
//        setArray[i] = red;//(iterations / n_iters) * 255;
//        setArray[i + 1] = green;
//        setArray[i + 2] = blue;
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot");
    sf::Texture texture;
    if (!texture.create(WIDTH, HEIGHT))
        return -1;
    bool generate_new = true;
    auto *pixels = new sf::Uint8[WIDTH * HEIGHT * 4];
    init_pixels(pixels);
    sf::Sprite sprite(texture);
    int n_iters = N_ITERS;
    long double divider = 1.;
    long double real_shift = 0.7;
    long double imag_shift = 0;
    while (window.isOpen())
    {
        sf::Event event{};
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::MouseWheelMoved:
                if (event.mouseWheel.delta == -1)
                    divider /= 1.01;
                else
                    divider *= 1.01;
                generate_new = true;
                break;
            case sf::Event::KeyPressed:
                switch (event.key.code) {
                case sf::Keyboard::Left:
                    real_shift += 0.1 / divider;
                    generate_new = true;
                    break;
                case sf::Keyboard::Right:
                    real_shift -= 0.1 / divider;
                    generate_new = true;
                    break;
                case sf::Keyboard::Up:
                    imag_shift += 0.1 / divider;
                    generate_new = true;
                    break;
                case sf::Keyboard::K:
                    divider *= 1.02;
                    std::cout << divider << std::endl;
                    generate_new = true;
                    break;
                case sf::Keyboard::J:
                    divider /= 1.02;
                    generate_new = true;
                    break;
                case sf::Keyboard::R:
                    divider = 1;
                    generate_new = true;
                    break;
                case sf::Keyboard::Down:
                    imag_shift -= 0.1 / divider;
                    generate_new = true;
                    break;
                case sf::Keyboard::H:
                    n_iters -= 10;
                    generate_new = true;
                    break;
                case sf::Keyboard::L:
                    n_iters += 10;
                    generate_new = true;
                    break;
                default:
                    break;
                }
            default:
                break;
            }
        }
        if (generate_new) {
            generate_set(pixels, divider, real_shift, imag_shift, n_iters);
            generate_new = false;
        }
        texture.update(pixels);
        window.draw(sprite);
//        window.clear();
        window.display();
    }
    delete[] pixels;
    return 0;
}