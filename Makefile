# Project: MandelbrotZoom Set Visualizer
# Author: Martin Kozlovsky
# Date: 22. 2. 2021

CPU_COMPILER = g++
GPU_COMPILER = pgc++
STANDARD = c++20
COMMON_FLAGS = -std=$(STANDARD) -lsfml-graphics -lsfml-window -lsfml-system
CPU_FLAGS = $(COMMON_FLAGS) -fopenmp -Ofast -ffast-math
GPU_FLAGS = $(COMMON_FLAGS) -acc -fast -O4 -DGPU
OUTPUT = mandelbrot

$(OUTPUT) : main.cpp classes.cpp classes.h
ifeq ($(ACC), gpu)
	$(GPU_COMPILER) $(GPU_FLAGS) -o $(OUTPUT)_gpu main.cpp classes.cpp
else
	$(CPU_COMPILER) $(CPU_FLAGS) -o $(OUTPUT)_cpu main.cpp classes.cpp
endif
