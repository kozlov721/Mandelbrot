#pragma once

#include <SFML/Graphics.hpp>
#include <string>

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
    sf::Uint8 *pixels = nullptr;
    int width;
    int height;
    int len;
    bool update = true;
    bool toggle_info = true;
    std::string info_string;

    void handle_events();
    void update_info();
    void create_pixels();
    void destroy_pixels();

public:
    Window(int width, int height, Fractal *fractal);
    ~Window();
    void run();
};
