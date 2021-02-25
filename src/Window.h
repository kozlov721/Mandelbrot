#pragma once

#include <SFML/Graphics.hpp>
#include "FractalHandler.h"

class Window {
    FractalHandler fractal;
    sf::RenderWindow window;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Clock clock;
    sf::Font font;
    sf::Text info;
    sf::RectangleShape info_background;
    sf::Uint8 *pixels;
    const int width;
    const int height;
    const int len;
    bool update = true;
    bool toggle_info = true;
    char info_string[300] = {'\0'};

    void handle_events();
    void update_info();
    void init_pixels();

public:
    Window(int width, int height, Fractal *fractal);
    ~Window();
    void run();
};
