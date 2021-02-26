#include <cassert>
#include "../include/Window.h"

#define MOVE_STEP 0.08
#define CHANNELS 4


Window::Window(int width, int height, Fractal *fractal):
        width(width), height(height), len(width * height * CHANNELS),
        fractal(FractalHandler(fractal)),
        window(sf::VideoMode(width, height), "MandelbrotZoom") {

    info_background.setSize(sf::Vector2f(
            float(width) / 7.f,
            float(height) / 40.f * (3.f + float(fractal->get_num_params())) * 1.2f
            ));
    window.setVerticalSyncEnabled(true);
    create_pixels();
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
    destroy_pixels();
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
            else if (key_code == sf::Keyboard::Num1)
                fractal.change_param(0, shift, ctrl);
            else if (key_code == sf::Keyboard::Num2)
                fractal.change_param(1, shift, ctrl);
            else if (key_code == sf::Keyboard::Num3)
                fractal.change_param(2, shift, ctrl);
            else if (key_code == sf::Keyboard::Num4)
                fractal.change_param(3, shift, ctrl);
            else if (key_code == sf::Keyboard::Num5)
                fractal.change_param(4, shift, ctrl);
            else if (key_code == sf::Keyboard::Num6)
                fractal.change_param(5, shift, ctrl);
            else if (key_code == sf::Keyboard::Num7)
                fractal.change_param(6, shift, ctrl);
            else if (key_code == sf::Keyboard::Num8)
                fractal.change_param(7, shift, ctrl);
            else if (key_code == sf::Keyboard::Num9)
                fractal.change_param(8, shift, ctrl);
            update = true;
        }
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && window.hasFocus()) {
        sf::Vector2i last_mouse_position = sf::Mouse::getPosition(window);
        if (last_mouse_position.y <= 0) {
            while (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                sf::sleep(sf::milliseconds(100));
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
            fractal.zoom(shift ? 1.05 : 1.03);
            update = true;
        } if (sf::Keyboard::isKeyPressed(sf::Keyboard::J)) {
            fractal.zoom(shift ? 1. / 1.05 : 1. / 1.03);
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
    info_string = "FPS: " + std::to_string(std::min(int(fps), 60)) + "\n"
                + fractal.generate_info_string();
    info.setString(std::string(info_string));
    clock.restart();
}

void Window::create_pixels() {
    pixels = new sf::Uint8[len];
    #pragma acc enter data create(pixels[0:len])
    for (int i = 0; i < width * height * 4; i += 4)
        pixels[i + 3] = 255;
    #pragma acc update device(pixels[0:len])
}

void Window::destroy_pixels() {
    #pragma acc exit data delete(pixels[0:len])
    delete[] pixels;
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
                if (update) {
                    update_info();
                    fractal.adjust_max_iters();
                    fractal.generate(int(width), int(height), pixels);
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
