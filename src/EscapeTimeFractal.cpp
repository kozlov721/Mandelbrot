#include <cmath>
#include <sstream>
#include <iomanip>

#include "../include/EscapeTimeFractal.h"

#define INITIAL_K 10.f
#define INITIAL_R 1.f
#define INITIAL_G 0.2357022604f
#define INITIAL_B 0.58490406881f

EscapeTimeFractal::EscapeTimeFractal() {
    params = new set_type[9];
    #pragma acc enter data copyin(this) create(params[0:9])
    params[0] = INITIAL_K;
    params[1] = INITIAL_R;
    params[2] = INITIAL_G;
    params[3] = INITIAL_B;
    #pragma acc update device(this->params[0:9])
}

EscapeTimeFractal::~EscapeTimeFractal() {
    #pragma acc exit data delete(this->params[0:9])
    delete[] params;
}

std::string EscapeTimeFractal::get_params_info() const {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) <<
    "K: " << params[0] << std::endl <<
    "R: " << params[1] << std::endl <<
    "G: " << params[2] << std::endl <<
    "B: " << params[3] << std::endl;
    return stream.str();
}

int EscapeTimeFractal::get_num_params() const {
    return 4;
}

bool EscapeTimeFractal::change_param(int param_index, bool fast, int direction) {
    if (param_index > 3) return false;
    set_type speed = fast ? 5. : 1.;
    set_type change = param_index == 0 ? 0.5 : 0.05;
    params[param_index] += direction * speed * change;
    return true;
}

void EscapeTimeFractal::reset_params() {
    params[0] = INITIAL_K;
    params[1] = INITIAL_R;
    params[2] = INITIAL_G;
    params[3] = INITIAL_B;
}
