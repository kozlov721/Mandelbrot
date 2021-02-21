#include "classes.h"
#include <cmath>
#include <fstream>
#include <cassert>

#define K_COLOR 10.
#define A_COLOR (1. * (1. / K_COLOR))
#define B_COLOR (0.2357022604 * (1. / K_COLOR))
#define C_COLOR (0.58490406881 * (1. / K_COLOR))

Mandelbrot::Mandelbrot(int w, int h) : width(w), height(h) {
    pixels = new sf::Uint8[w * h * 4];
    init_pixels();
}

Mandelbrot::~Mandelbrot() {
    delete[] pixels;
}

void Mandelbrot::init_pixels() {
    for (int i = 0; i < width * height * 4; i += 4)
        pixels[i + 3] = 255;
}

Mandelbrot::Mandelbrot(int w, int h, int max_i,
                       set_type d_r, set_type d_i, set_type z):
                       Mandelbrot(w, h) {
    max_iters = max_i;
    real_shift = d_r;
    imag_shift = d_i;
    zoom = z;
}

int Mandelbrot::iterate(set_type c_r, set_type c_j) const {
    set_type z_r = 0;
    set_type z_j = 0;
    set_type new_r;
    unsigned short i;
    for (i = 0; i < max_iters; ++i) {
        new_r = z_r * z_r - z_j * z_j;
        z_j = 2 * z_r * z_j;
        z_r = new_r;
        z_r += c_r;
        z_j += c_j;
        if (z_r * z_r + z_j * z_j >= 4.)
            break;
    }
    return i;
}

inline void Mandelbrot::fill_color(const unsigned short iters, const unsigned int index) {
    pixels[index] = short(255 * ((1 - std::cos(A_COLOR * iters)) / 2));
    pixels[index + 1] = short(255 * ((1 - std::cos(B_COLOR * iters)) / 2));
    pixels[index + 2] = short(255 * ((1 - std::cos(C_COLOR * iters)) / 2));
}

void Mandelbrot::generate() {
    #pragma omp parallel for num_threads(64)
    for (int i = 0; i < width * height * 4; i += 4) {
        int j = (i / 4) / width;
        int r = (i / 4) % width;
        set_type c_r = (((set_type) r / width - 0.5) * 3) / zoom - real_shift;
        set_type c_j = (((set_type) j / height - 0.5) * 2) / zoom - imag_shift;
        int iterations = iterate(c_r, c_j);
        fill_color(iterations, i);
    }
}

void Mandelbrot::save(const std::string &name) const {
    Mandelbrot high_res_set(WIDTH_8K, HEIGHT_8K, max_iters,
                            real_shift, imag_shift, zoom);
    high_res_set.generate();
    sf::Uint8 *high_res_pixels = high_res_set.get_pixels();
    std::ofstream out(name, std::ios::binary | std::ios::out | std::ios::trunc);
    out << "P6\n" << WIDTH_8K << " " << HEIGHT_8K << "\n255\n";
    for (int i = 0; i < WIDTH_8K * HEIGHT_8K * 4; i += 4) {
        for (int j = 0; j < 3; ++j) {
            const char pixel = high_res_pixels[i + j];
            out.write(&pixel, 1);
        }
    }
}

void Mandelbrot::move(set_type d_r, set_type d_i) {
    real_shift += d_r / zoom;
    imag_shift += d_i / zoom;
}

void Mandelbrot::change_iter_speed(float change) {
    iter_speed += change;
}

void Mandelbrot::zoom_in() {
    zoom *= 1.02;
}

void Mandelbrot::zoom_out() {
    zoom /= 1.02;
}

void Mandelbrot::move_left() {
    move(0.1, 0);
}

void Mandelbrot::move_right() {
    move(-0.1, 0);
}

void Mandelbrot::move_up() {
    move(0, 0.1);
}

void Mandelbrot::move_down() {
    move(0, -0.1);
}

void Mandelbrot::reset(bool reset_speed) {
    zoom = INITIAL_ZOOM;
    iter_speed = reset_speed ? INITIAL_ITER_SPEED : iter_speed;
}

void Mandelbrot::adjust_max_iters() {
    max_iters = std::max(INITIAL_MAX_ITERS,
                         int(std::log(zoom) * iter_speed));
}

set_type Mandelbrot::get_zoom() const {
    return zoom;
}

sf::Uint8 *Mandelbrot::get_pixels() {
    return pixels;
}

/*----------------------------------------------*/

Visualizer::Visualizer(int width, int height) :
        window(sf::RenderWindow(sf::VideoMode(width, height),
                                "Mandelbrot")),
        set(Mandelbrot(width, height)) {

    window.setFramerateLimit(60);
    texture.create(width, height);
    sprite = sf::Sprite(texture);
    assert(font.loadFromFile("arial.ttf"));
    info.setFont(font);
    info.setString("60\n1");
    info.setCharacterSize(height / 25);
    info.setFillColor(sf::Color::Red);
}

void Visualizer::handle_events() {
    sf::Event event{};
    while (window.pollEvent(event)) {
        switch (event.type) {
        case sf::Event::Closed:
            window.close();
            break;
        case sf::Event::KeyPressed:
            switch (event.key.code) {
            case sf::Keyboard::Left:
                set.move_left();
                break;
            case sf::Keyboard::Right:
                set.move_right();
                break;
            case sf::Keyboard::Up:
                set.move_up();
                break;
            case sf::Keyboard::Down:
                set.move_down();
                break;
            case sf::Keyboard::K:
                set.zoom_in();
                break;
            case sf::Keyboard::J:
                set.zoom_out();
                break;
            case sf::Keyboard::R:
                set.reset(false);
                break;
            case sf::Keyboard::B:
                set.reset(true);
                break;
            case sf::Keyboard::H:
                set.change_iter_speed(-0.2);
                break;
            case sf::Keyboard::L:
                set.change_iter_speed(0.2);
                break;
            case sf::Keyboard::S:
                set.save("image.ppm");
                break;
            default:
                break;
            }
            update = true;
            break;
        default:
            break;
        }
    }
}

void Visualizer::update_info() {
    float fps = 1.f / clock.getElapsedTime().asSeconds();
    sprintf(info_string, "%d\n%.3Le", std::min(int(fps), 60), set.get_zoom());
    info.setString(std::string(info_string));
    clock.restart();
}

void Visualizer::run() {
    while (window.isOpen()) {
        handle_events();
        if (update) {
            set.adjust_max_iters();
            set.generate();
            update = false;
        }
        texture.update(set.get_pixels());
        update_info();
        window.clear();
        window.draw(sprite);
        window.draw(info);
        window.display();
    }
}
