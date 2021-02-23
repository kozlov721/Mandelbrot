#pragma once

#include <SFML/Graphics.hpp>
#include <string>


#define INITIAL_MAX_ITERS 30
#define INITIAL_ITER_SHIFT 0
#define INITIAL_REAL_SHIFT .7
#define INITIAL_IMAG_SHIFT .0
#define INITIAL_ZOOM 1.
#define INITIAL_K 10.f
#define INITIAL_R 1.f
#define INITIAL_G 0.2357022604f
#define INITIAL_B 0.58490406881f


#ifdef GPU
typedef double set_type;
#else
typedef long double set_type;
#endif

class Mandelbrot {
private:
    float k_constant = INITIAL_K;
    float r_constant = INITIAL_R;
    float g_constant = INITIAL_G;
    float b_constant = INITIAL_B;
    int max_iterations_shift = INITIAL_ITER_SHIFT;
public:
    int getMaxIters() const;

private:
    int max_iters = INITIAL_MAX_ITERS;
    set_type zoom_value = INITIAL_ZOOM;
    set_type real_shift = INITIAL_REAL_SHIFT;
    set_type imag_shift = INITIAL_IMAG_SHIFT;
    int width;
    int height;
    int len;
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
    void generate();
    void adjust_max_iters();
    void save(const std::string &name) const;
    void change_iter_shift(int change);
    void zoom(bool in, bool fast);
    void move_left();
    void move_right();
    void move_up();
    void move_down();
    void reset(bool reset_zoom, bool reset_iters, bool reset_colors);

    set_type get_zoom() const;
    sf::Uint8 *get_pixels();
    float get_k_constant() const;
    void set_k_constant(float k_constant);
    float get_r_constant() const;
    void set_r_constant(float r_constant);
    float get_g_constant() const;
    void set_g_constant(float g_constant);
    float get_b_constant() const;
    void set_b_constant(float b_constant);
};

/*---------------------------------------*/

class Visualizer {
private:
    Mandelbrot set;
    sf::RenderWindow window;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Clock clock;
    sf::Font font;
    sf::Text info;
    sf::RectangleShape info_background;
    bool update = true;
    bool toggle_info = true;
    char info_string[300] = {'\0'};

    void handle_events();
    void update_info();

public:
    Visualizer(int width, int height);
    void run();
};
