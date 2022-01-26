#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "Mandelbrot.h"

class Window {
    Mandelbrot fractal = Mandelbrot();
    sf::RenderWindow window;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Clock clock;
    sf::Font font;
    sf::Text info;
    sf::RectangleShape info_background;
    sf::Uint8 *pixels = nullptr;
    std::string info_string;
    int width;
    int height;
    int len;
    bool update = true;
    bool toggle_info = true;

    void handle_events();
    void update_info();
    void create_pixels();
    void capture(const std::string &name) const;

public:
    Window(int width, int height);
    ~Window();
    void run();
};
