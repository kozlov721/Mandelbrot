# Project: MandelbrotZoom Set Visualizer
# Author: Martin Kozlovsky
# Date: 22. 2. 2021

CPU_COMPILER = g++
GPU_COMPILER = pgc++
STANDARD = c++20
COMMON_FLAGS = -std=$(STANDARD) -lsfml-graphics -lsfml-window -lsfml-system -fopenmp
CPU_FLAGS = $(COMMON_FLAGS) -Ofast -ffast-math
GPU_FLAGS = $(COMMON_FLAGS) -acc -fast -O4 -DGPU
OUTPUT = mandelbrot

$(OUTPUT) : src/Main.cpp src/classes.cpp src/classes.h
ifeq ($(ACC), gpu)
	$(GPU_COMPILER) -o $(OUTPUT)_gpu src/main.cpp src/Window.cpp src/AbstractFractal.cpp src/FractalHandler.cpp src/Fractals.cpp $(GPU_FLAGS)
else
	$(CPU_COMPILER) -o $(OUTPUT)_cpu src/main.cpp src/classes.cpp $(CPU_FLAGS)
endif
	rm main.o classes.o
