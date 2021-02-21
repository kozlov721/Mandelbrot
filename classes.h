#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#define WIDTH_8K 3840 * 2
#define HEIGHT_8K 2160 * 2
#define INITIAL_MAX_ITERS 30
#define INITIAL_ITER_SPEED 15
#define INITIAL_ZOOM 1.

typedef long double set_type;

class Mandelbrot {
    int width;
    int height;
    int max_iters = INITIAL_MAX_ITERS;
    float iter_speed = INITIAL_ITER_SPEED;
    set_type zoom = INITIAL_ZOOM;
    set_type real_shift = .7;
    set_type imag_shift = 0.;
    sf::Uint8 *pixels;

    void init_pixels();
    [[nodiscard]] int iterate(set_type c_r, set_type c_j) const;
    void fill_color(unsigned short iters, unsigned int index);
    void move(set_type d_r, set_type d_i);

public:
    Mandelbrot(int width, int height);
    Mandelbrot(int width, int height, int max_iters,
               set_type real_shift, set_type imag_shift, set_type zoom);
    ~Mandelbrot();

    void generate();
    void adjust_max_iters();
    void save(const std::string &name) const;
    void change_iter_speed(float change);
    [[nodiscard]] set_type get_zoom() const;
    [[nodiscard]] sf::Uint8 *get_pixels();
    void zoom_in();
    void zoom_out();
    void move_left();
    void move_right();
    void move_up();
    void move_down();
    void reset(bool reset_speed);
};

/*---------------------------------------*/

class Visualizer {
    Mandelbrot set;
    sf::RenderWindow window;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Clock clock;
    sf::Font font;
    sf::Text info;
    bool update = true;
    char info_string[100] = {'\0'};

    void handle_events();
    void update_info();

public:
    Visualizer(int width, int height);
    void run();
};