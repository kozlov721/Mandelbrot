#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#define INITIAL_MAX_ITERS 30
#define INITIAL_ITER_SHIFT 0
#define INITIAL_ZOOM 1.

#ifdef GPU
typedef double set_type;
#else
typedef long double set_type;
#endif

class Mandelbrot {
private:
    float k_constant = 10.;
    float r_constant = 1.f;
    float g_constant = 0.2357022604f;
    float b_constant = 0.58490406881f;
    int max_iterations_shift = INITIAL_ITER_SHIFT;
    int width;
    int height;
    int len;
    int max_iters = INITIAL_MAX_ITERS;
    set_type zoom = INITIAL_ZOOM;
    set_type real_shift = .7;
    set_type imag_shift = .0;
    sf::Uint8 *pixels;

    void init_pixels();
    int iterate(set_type c_r, set_type c_j) const;
    void fill_color(unsigned short iters, unsigned int index);
    void move(set_type d_r, set_type d_i);

public:
    Mandelbrot(int width, int height);
    Mandelbrot(int width, int height, int max_iters,
               set_type real_shift, set_type imag_shift, set_type zoom);
    ~Mandelbrot();
//    #pragma acc routine
    void generate();
    void adjust_max_iters();
    void save(const std::string &name) const;
    void change_iter_shift(int change);
    set_type get_zoom() const;
    sf::Uint8 *get_pixels();
    void zoom_in();
    void zoom_out();
    void move_left();
    void move_right();
    void move_up();
    void move_down();
    void reset(bool reset_speed);

    float getKConstant() const;
    void setKConstant(float kConstant);
    float getRConstant() const;
    void setRConstant(float rConstant);
    float getGConstant() const;
    void setGConstant(float gConstant);
    float getBConstant() const;
    void setBConstant(float bConstant);
};

/*---------------------------------------*/

class Visualizer {
private:
    Mandelbrot set;
    sf::RenderWindow *window;
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
    ~Visualizer();
    void run();
};
