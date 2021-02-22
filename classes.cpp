#include "classes.h"
#include <cmath>
#include <fstream>
#include <cassert>
#include <iostream>

#define SHIFT 0.08
#define ITER_SPEED 15
#define WIDTH_8K 3840 * 2
#define HEIGHT_8K (WIDTH_8K * 2 / 3)

Mandelbrot::Mandelbrot(int w, int h) : width(w), height(h), len(w * h * 4) {
    pixels = new sf::Uint8[len];
    init_pixels();
#ifdef GPU
    #pragma acc enter data copyin(this) create(pixels[0:len])
    #pragma acc update device(this->pixels[0:this->len])
#endif

}

Mandelbrot::~Mandelbrot() {
#ifdef GPU
    #pragma acc exit data delete(this, pixels[0:len])
#endif
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

inline int Mandelbrot::iterate(set_type c_r, set_type c_j) const {
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
    pixels[index] = short(255 * ((1 - std::cos(r_constant * float(iters) / k_constant)) / 2));
    pixels[index + 1] = short(255 * ((1 - std::cos(g_constant * float(iters) / k_constant)) / 2));
    pixels[index + 2] = short(255 * ((1 - std::cos(b_constant * float(iters) / k_constant)) / 2));
}

void Mandelbrot::generate() {
#ifdef GPU
    #pragma acc update device(this->zoom, this->max_iters, \
                              this->real_shift, this->imag_shift, \
                              this->k_constant, this->r_constant, \
                              this->g_constant, this->b_constant)

    #pragma acc data present(pixels[0:len]) copyout(pixels[0:len])
    {
    #pragma acc parallel loop
#else
    #pragma omp parallel for num_threads(64)
#endif
    for (int i = 0; i < len; i += 4) {
        int j = (i / 4) / width;
        int r = (i / 4) % width;
        set_type c_r = (((set_type) r / width - 0.5) * 3) / zoom - real_shift;
        set_type c_j = (((set_type) j / height - 0.5) * 2) / zoom - imag_shift;
        int iterations = iterate(c_r, c_j);
        fill_color(iterations, i);
    }
#ifdef GPU
    #pragma acc update self(this->pixels[0:this->len])
    }
#endif
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

void Mandelbrot::change_iter_shift(int change) {
    max_iterations_shift += change;
}

void Mandelbrot::zoom_in() {
    zoom *= 1.02;
}

void Mandelbrot::zoom_out() {
    zoom /= 1.02;
}

void Mandelbrot::move_left() {
    move(SHIFT, 0);
}

void Mandelbrot::move_right() {
    move(-SHIFT, 0);
}

void Mandelbrot::move_up() {
    move(0, SHIFT);
}

void Mandelbrot::move_down() {
    move(0, -SHIFT);
}

void Mandelbrot::reset(bool reset_iters) {
    zoom = INITIAL_ZOOM;
    max_iterations_shift = reset_iters ? INITIAL_ITER_SHIFT : max_iterations_shift;
}

void Mandelbrot::adjust_max_iters() {
    max_iters = std::max(INITIAL_MAX_ITERS,
                         int(std::log(zoom) * ITER_SPEED) + max_iterations_shift);
}

set_type Mandelbrot::get_zoom() const {
    return zoom;
}

sf::Uint8 *Mandelbrot::get_pixels() {
    return pixels;
}

float Mandelbrot::getKConstant() const {
    return k_constant;
}

void Mandelbrot::setKConstant(float kConstant) {
    k_constant = kConstant;
}

float Mandelbrot::getRConstant() const {
    return r_constant;
}

void Mandelbrot::setRConstant(float rConstant) {
    r_constant = rConstant;
}

float Mandelbrot::getGConstant() const {
    return g_constant;
}

void Mandelbrot::setGConstant(float gConstant) {
    g_constant = gConstant;
}

float Mandelbrot::getBConstant() const {
    return b_constant;
}

void Mandelbrot::setBConstant(float bConstant) {
    b_constant = bConstant;
}

/*----------------------------------------------*/

Visualizer::Visualizer(int width, int height) :
        set(Mandelbrot(width, height)),
        window(sf::VideoMode(width, height), "Mandelbrot"){
//    window = new sf::RenderWindow(sf::VideoMode(width, height), "Mandelbrot");
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
        int shift = sf::Keyboard::isKeyPressed(
                sf::Keyboard::LShift
        ) ? -1 : 1;
        switch (event.type) {
        case sf::Event::Closed:
            window.close();
            break;
        case sf::Event::KeyPressed:
            switch (event.key.code) {
            case sf::Keyboard::A:
                set.move_left();
                break;
            case sf::Keyboard::D:
                set.move_right();
                break;
            case sf::Keyboard::W:
                set.move_up();
                break;
            case sf::Keyboard::S:
                set.move_down();
                break;
            case sf::Keyboard::K:
                set.zoom_in();
                break;
            case sf::Keyboard::J:
                set.zoom_out();
                break;
            case sf::Keyboard::R:
                set.reset(true);
                break;
            case sf::Keyboard::B:
                set.reset(false);
                break;
            case sf::Keyboard::H:
                set.change_iter_shift(-5);
                break;
            case sf::Keyboard::L:
                set.change_iter_shift(5);
                break;
            case sf::Keyboard::U:
                set.setKConstant(set.getKConstant() + float(shift) * 1);
                break;
            case sf::Keyboard::I:
                set.setRConstant(set.getRConstant() + float(shift) * 0.1f);
                break;
            case sf::Keyboard::O:
                set.setGConstant(set.getGConstant() + float(shift) * 0.1f);
                break;
            case sf::Keyboard::P:
                set.setBConstant(set.getBConstant() + float(shift) * 0.1f);
                break;
            case sf::Keyboard::Enter:
                set.save("image" + std::to_string(set.get_zoom()) + ".ppm");
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
    sprintf(info_string, "%d\n%.3Le", std::min(int(fps), 60), (long double) set.get_zoom());
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
