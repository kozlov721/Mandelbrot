#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

#define WIDTH (300 * 2)
#define HEIGHT (200 * 2)
#define N_ITERS 1000
#define K_COLOR 10.
#define A_COLOR (1. * (1. / K_COLOR))
#define B_COLOR (0.2357022604 * (1. / K_COLOR))
#define C_COLOR (0.58490406881 * (1. / K_COLOR))

using set_type = long double;

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

void init_pixels(sf::Uint8 *setArray) {
    for (int i = 0; i < WIDTH * HEIGHT * 4; i += 4)
        setArray[i + 3] = 255;
}
inline void color(unsigned short iters, sf::Uint8 &r, sf::Uint8 &g, sf::Uint8 &b) {
    r = std::floor(255 * ((1 - std::cos(A_COLOR * iters)) / 2));
    g = std::floor(255 * ((1 - std::cos(B_COLOR * iters)) / 2));
    b = std::floor(255 * ((1 - std::cos(C_COLOR * iters)) / 2));
}

void generate_set(sf::Uint8 *setArray, const set_type divider,
                  const set_type real_shift, const set_type imag_shift,
                  const int n_iters) {
    #pragma omp parallel for num_threads(64)
    for (int i = 0; i < WIDTH * HEIGHT * 4; i += 4) {
        int j = (i / 4) / WIDTH;
        int r = (i / 4) % WIDTH;
        set_type c_r = (((set_type) r / WIDTH - 0.5) * 3) / divider - real_shift;
        set_type c_j = (((set_type) j / HEIGHT - 0.5) * 2) / divider - imag_shift;
        int iterations = iterate(c_r, c_j, n_iters);
        color(iterations, setArray[i], setArray[i + 1], setArray[i + 2]);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot");
    sf::Texture texture;
    texture.create(WIDTH, HEIGHT);
    bool generate_new = true;
    auto *pixels = new sf::Uint8[WIDTH * HEIGHT * 4];
    init_pixels(pixels);
    sf::Sprite sprite(texture);
    unsigned short n_iters = N_ITERS;
    set_type divider = 1.;
    set_type real_shift = 0.7;
    set_type imag_shift = 0;

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
                    divider /= 1.02;
                else
                    divider *= 1.02;
                generate_new = true;
                break;
            case sf::Event::KeyPressed:
                switch (event.key.code) {
                case sf::Keyboard::Left:
                    real_shift += 0.1 / divider;
                    break;
                case sf::Keyboard::Right:
                    real_shift -= 0.1 / divider;
                    break;
                case sf::Keyboard::Up:
                    imag_shift += 0.1 / divider;
                    break;
                case sf::Keyboard::K:
                    divider *= 1.02;
                    break;
                case sf::Keyboard::J:
                    divider /= 1.02;
                    break;
                case sf::Keyboard::R:
                    divider = 1;
                    break;
                case sf::Keyboard::Down:
                    imag_shift -= 0.1 / divider;
                    break;
                case sf::Keyboard::H:
                    n_iters -= 10;
                    if (n_iters <= 0)
                        n_iters = 10;
                    break;
                case sf::Keyboard::L:
                    n_iters += 10;
                    break;
                default:
                    break;
                }
                generate_new = true;
                break;
            default:
                break;
            }
        }
        if (generate_new) {
            n_iters = std::max(30, int(std::log(divider) * 40));
            generate_set(pixels, divider, real_shift, imag_shift, n_iters);
            generate_new = false;
            std::cout << divider << "  " << n_iters << "  " << std::log(divider) << std::endl;
        }
        texture.update(pixels);
        window.draw(sprite);
        window.display();
    }
    delete[] pixels;
    return 0;
}