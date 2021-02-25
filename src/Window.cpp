#include "Window.h"
#include <cassert>
#define MOVE_STEP 0.08
#define CHANNELS 4
#include <iostream>
Window::Window(int width, int height, Fractal *fractal):
        width(width), height(height), len(width * height * CHANNELS),
        fractal(FractalHandler(width, height, fractal)),
        window(sf::VideoMode(width, height), "MandelbrotZoom"),
        info_background(sf::Vector2f(float(width) / 7,
                                         float(height) / 40 * 8.5)) {
    window.setVerticalSyncEnabled(true);
    pixels = new sf::Uint8[len];
    init_pixels();
    #pragma acc enter data create(pixels[0:len])
    #pragma acc update device(pixels[0:len])
    texture.create(width, height);
    info_background.setFillColor(sf::Color::Black);
    sprite = sf::Sprite(texture);
    assert(font.loadFromFile("fonts/arial.ttf"));
    info.setFont(font);
    info.setString("60\n1");
    info.setCharacterSize(height / 40);
    info.setFillColor(sf::Color::White);
}

Window::~Window() {
    #pragma acc exit data delete(pixels[0:len])
    delete[] pixels;
}

void Window::handle_events() {
    bool shift = sf::Keyboard::isKeyPressed(
            sf::Keyboard::LShift
    ) || sf::Keyboard::isKeyPressed(
            sf::Keyboard::RShift);

    bool ctrl = sf::Keyboard::isKeyPressed(
            sf::Keyboard::LControl
    ) || sf::Keyboard::isKeyPressed(
            sf::Keyboard::RControl);

    sf::Event event{};
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        else if (event.type == sf::Event::KeyPressed) {
            auto key_code = event.key.code;
            if (key_code == sf::Keyboard::Escape)
                window.close();
            else if (key_code == sf::Keyboard::R)
                fractal.reset(true, true);
            else if (key_code == sf::Keyboard::B)
                fractal.reset(true, false);
            else if (key_code == sf::Keyboard::T)
                toggle_info = !toggle_info;
            else if (key_code == sf::Keyboard::Enter)
                fractal.save("mandelbrot" + std::to_string(fractal.get_zoom()) + ".ppm");
            update = true;
        }
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && window.hasFocus()) {
        sf::Vector2i last_mouse_position = sf::Mouse::getPosition(window);
        if (last_mouse_position.y <= 0) {
            while (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                sf::sleep(sf::milliseconds(50));
            return;
        }
        sf::sleep(sf::milliseconds(20));
        sf::Vector2i current_position = sf::Mouse::getPosition(window);
        int d_x = current_position.x - last_mouse_position.x;
        int d_y = current_position.y - last_mouse_position.y;
        set_type d_r = set_type(d_x) / width * 3;
        set_type d_i = set_type(d_y) / height * 2;
        fractal.move(d_r * 1.5,
                     d_i * 1.5);
        update = true;
    }
    if (window.hasFocus()) {
//        sf::sleep(sf::milliseconds(40));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)
                || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            fractal.move(0, MOVE_STEP);
            update = true;
        } if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)
                || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            fractal.move(0, -MOVE_STEP);
            update = true;
        } if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)
                || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            fractal.move(MOVE_STEP, 0);
            update = true;
        } if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)
                || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            fractal.move(-MOVE_STEP, 0);
            update = true;
        } if (sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {
            fractal.zoom(ctrl ? 1.05 : 1.03);
            update = true;
        } if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)) {
            fractal.zoom(ctrl ? 1. / 1.05 : 1. / 1.03);
            update = true;
        } if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) {
            fractal.change_iter_shift(-5);
            update = true;
        } if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
            fractal.change_iter_shift(5);
            update = true;
        }
    }
}

void Window::update_info() {
    float fps = 1.f / clock.getElapsedTime().asSeconds();
    sprintf(info_string, "FPS: %d\n"
                         "Zoom: %.3Le\n"
                         "Iterations: %d\n",
            std::min(int(fps), 60), (long double) fractal.get_zoom(),
            fractal.get_max_iters());
    info.setString(std::string(info_string));
    clock.restart();
}

void Window::run() {
    while (window.isOpen()) {
        #pragma omp parallel sections num_threads(2)
        {
            #pragma omp section
            {
                handle_events();
            }
            #pragma omp section
            {
                update_info();
                if (update) {
                    fractal.adjust_max_iters();
                    fractal.generate(pixels);
                    update = false;
                }
            }
        }
        texture.update(pixels);
        window.clear();
        window.draw(sprite);
        if (toggle_info) {
            window.draw(info_background);
            window.draw(info);
        }
        window.display();
    }
}

void Window::init_pixels() {
    for (int i = 0; i < width * height * 4; i += 4)
        pixels[i + 3] = 255;
}
