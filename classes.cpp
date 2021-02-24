#include "classes.h"
#include <cmath>
#include <fstream>
#include <cassert>
#include <iostream>

#define MOVE_STEP 0.08
#ifdef GPU
    #define ITER_SPEED 25
#else
    #define ITER_SPEED 15
#endif
#define WIDTH_8K (3840 * 2)
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
    zoom_value = z;
}

inline int Mandelbrot::iterate(const set_type &c_r, const set_type &c_j) const {
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

inline void Mandelbrot::fill_color(int iters, int index) {
    pixels[index] = short(255 * ((1 - std::cos(r_constant * float(iters) / k_constant)) / 2));
    pixels[index + 1] = short(255 * ((1 - std::cos(g_constant * float(iters) / k_constant)) / 2));
    pixels[index + 2] = short(255 * ((1 - std::cos(b_constant * float(iters) / k_constant)) / 2));
}

void Mandelbrot::generate() {
#ifdef GPU
    #pragma acc update device(this->zoom_value, this->max_iters, \
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
        set_type c_r = (((set_type) r / width - 0.5) * 3) / zoom_value - real_shift;
        set_type c_j = (((set_type) j / height - 0.5) * 2) / zoom_value - imag_shift;
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
                            real_shift, imag_shift, zoom_value);
    high_res_set.set_k_constant(k_constant);
    high_res_set.set_r_constant(r_constant);
    high_res_set.set_g_constant(g_constant);
    high_res_set.set_b_constant(b_constant);
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
    real_shift += d_r / zoom_value;
    imag_shift += d_i / zoom_value;
}

void Mandelbrot::change_iter_shift(int change) {
    max_iterations_shift += change;
}

void Mandelbrot::zoom(bool in, bool fast) {
    set_type speed = fast ? 1.04 : 1.02;
    if (in)
        zoom_value *= speed;
    else
        zoom_value /= speed;
}

void Mandelbrot::reset(bool reset_zoom, bool reset_iters, bool reset_colors) {
    zoom_value = reset_zoom ? INITIAL_ZOOM : zoom_value;
    max_iterations_shift = reset_iters ? INITIAL_ITER_SHIFT : max_iterations_shift;
    if (reset_colors) {
        k_constant = INITIAL_K;
        r_constant = INITIAL_R;
        g_constant = INITIAL_G;
        b_constant = INITIAL_B;
    }
}

void Mandelbrot::adjust_max_iters() {
    max_iters = std::max(INITIAL_MAX_ITERS,
                         int(std::log(zoom_value) * ITER_SPEED) + max_iterations_shift);
}

set_type Mandelbrot::get_zoom() const {
    return zoom_value;
}

sf::Uint8 *Mandelbrot::get_pixels() {
    return pixels;
}

float Mandelbrot::get_k_constant() const {
    return k_constant;
}

void Mandelbrot::set_k_constant(float k) {
    k_constant = k;
}

float Mandelbrot::get_r_constant() const {
    return r_constant;
}

void Mandelbrot::set_r_constant(float r) {
    r_constant = r;
}

float Mandelbrot::get_g_constant() const {
    return g_constant;
}

void Mandelbrot::set_g_constant(float g) {
    g_constant = g;
}

float Mandelbrot::get_b_constant() const {
    return b_constant;
}

void Mandelbrot::set_b_constant(float b) {
    b_constant = b;
}

int Mandelbrot::get_max_iters() const {
    return max_iters;
}

int Mandelbrot::get_width() const {
    return width;
}

int Mandelbrot::get_height() const {
    return height;
}

/*----------------------------------------------*/

Visualizer::Visualizer(int width, int height) :
        set(Mandelbrot(width, height)),
        window(sf::VideoMode(width, height), "MandelbrotZoom"),
        info_background(sf::Vector2f(float(width) / 7,
                                         float(height) / 40 * 8.5)) {
    texture.create(width, height);
    info_background.setFillColor(sf::Color::Black);
    sprite = sf::Sprite(texture);
    assert(font.loadFromFile("fonts/arial.ttf"));
    info.setFont(font);
    info.setString("60\n1");
    info.setCharacterSize(height / 40);
    info.setFillColor(sf::Color::White);

}

void Visualizer::handle_events() {

    float shift = sf::Keyboard::isKeyPressed(
            sf::Keyboard::LShift
    ) || sf::Keyboard::isKeyPressed(
            sf::Keyboard::RShift
    ) ? -1.f : 1.f;
    float speed = sf::Keyboard::isKeyPressed(
            sf::Keyboard::LControl
    ) || sf::Keyboard::isKeyPressed(
            sf::Keyboard::RControl
    ) ? 0.1f : 1.f;

    sf::Event event{};
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        else if (event.type == sf::Event::KeyPressed) {
            auto key_code = event.key.code;
#ifndef GPU
            if (key_code == sf::Keyboard::W
                    || key_code == sf::Keyboard::Up)
                set.move(0, MOVE_STEP);
            else if (key_code == sf::Keyboard::S
                    || key_code == sf::Keyboard::Down)
                set.move(0, -MOVE_STEP);
            else if (key_code == sf::Keyboard::A
                    || key_code == sf::Keyboard::Left)
                set.move(MOVE_STEP, 0);
            else if (key_code == sf::Keyboard::D
                    || key_code == sf::Keyboard::Right)
                set.move(-MOVE_STEP, 0);
            else if (key_code == sf::Keyboard::K)
                set.zoom(true, shift < 0);
            else if (key_code == sf::Keyboard::J)
                set.zoom(false, shift < 0);
            else if (key_code == sf::Keyboard::H)
                set.change_iter_shift(-5);
            else if (key_code == sf::Keyboard::L)
                set.change_iter_shift(5);
            else
#endif
            if (key_code == sf::Keyboard::R)
                set.reset(true, true, false);
            else if (key_code == sf::Keyboard::B)
                set.reset(true, false, false);
            else if (key_code == sf::Keyboard::C)
                set.reset(false, false, true);
            else if (key_code == sf::Keyboard::U)
                set.set_k_constant(set.get_k_constant() + speed * shift * 0.5f);
            else if (key_code == sf::Keyboard::I)
                set.set_r_constant(set.get_r_constant() + speed * shift * 0.1f);
            else if (key_code == sf::Keyboard::O)
                set.set_g_constant(set.get_g_constant() + speed * shift * 0.1f);
            else if (key_code == sf::Keyboard::P)
                set.set_b_constant(set.get_b_constant() + speed * shift * 0.1f);
            else if (key_code == sf::Keyboard::T)
                toggle_info = !toggle_info;
            else if (key_code == sf::Keyboard::Enter)
                set.save("mandelbrot" + std::to_string(set.get_zoom()) + ".ppm");
            update = true;
        }
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && window.hasFocus()) {
        sf::Vector2i last_mouse_position = sf::Mouse::getPosition(window);
        sf::sleep(sf::milliseconds(10));
        auto current_position = sf::Mouse::getPosition(window);
        int d_x = current_position.x - last_mouse_position.x;
        int d_y = current_position.y - last_mouse_position.y;
        set_type d_r = set_type(d_x) / set.get_width() * 3;
        set_type d_i = set_type(d_y) / set.get_height() * 2;
        set.move(d_r * 1.5,
                 d_i * 1.5);
        update = true;
    }
#ifdef GPU
    if (window.hasFocus()) {
//        sf::sleep(sf::milliseconds(5));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)
                || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            set.move(0, MOVE_STEP);
            update = true;
        } if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)
                || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            set.move(0, -MOVE_STEP);
            update = true;
        } if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)
                || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            set.move(MOVE_STEP, 0);
            update = true;
        } if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)
                || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            set.move(-MOVE_STEP, 0);
            update = true;
        } if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
            set.zoom(true, shift < 0);
            update = true;
        } if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)) {
            set.zoom(false, shift < 0);
            update = true;
        } if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) {
            set.change_iter_shift(-5);
            update = true;
        } if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
            set.change_iter_shift(5);
            update = true;
        }
    }
#endif
}

void Visualizer::update_info() {
    float fps = 1.f / clock.getElapsedTime().asSeconds();
    sprintf(info_string, "FPS: %d\n"
                         "Zoom: %.3Le\n"
                         "Iterations: %d\n"
                         "K: %.2f\n"
                         "R: %.2f\n"
                         "G: %.2f\n"
                         "B: %.2f",
            std::min(int(fps), 60), (long double) set.get_zoom(),
            set.get_max_iters(), set.get_k_constant(), set.get_r_constant(),
            set.get_g_constant(), set.get_b_constant());
    info.setString(std::string(info_string));
    clock.restart();
}

void Visualizer::run() {
    while (window.isOpen()) {
#ifdef GPU
        #pragma omp parallel sections num_threads(2)
        {
            #pragma omp section
            {
#endif
                handle_events();
#ifdef GPU
            }
            #pragma omp section
            {
#endif
                if (update) {
                    set.adjust_max_iters();
                    set.generate();
                    update = false;
                }
#ifdef GPU
            }
        }


#pragma omp single
#endif
        {
            texture.update(set.get_pixels());
            window.clear();
            window.draw(sprite);
            if (toggle_info) {
                update_info();
                window.draw(info_background);
                window.draw(info);
            }
            window.display();
        }
    }
}
