cmake_minimum_required(VERSION 3.17)
project(Mandelbrot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-lsfml-graphics -lsfml-window -lsfml-system -fopenmp -Ofast")
add_executable(Mandelbrot main.cpp)