# Project: Mandelbrot
# Author: Martin Kozlovsky
# Date: 22. 2. 2021

CXX = pgc++
CXXFLAGS = -std=c++20 -g -fast -O4 -acc
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -fopenmp -acc
NAME = Mandelbrot
SRC_DIR = src
BUILD_DIR = build
SRCS := $(shell find $(SRC_DIR) -name *.cpp)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

all: $(BUILD_DIR)/$(NAME)

dep:
	$(CXX) -MM src/*.cpp > $(BUILD_DIR)/dep.list


$(BUILD_DIR)/$(NAME): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean run
clean:
	rm -r $(BUILD_DIR)

run: $(BUILD_DIR)/$(NAME)
	./$(BUILD_DIR)/$(NAME)

-include $(BUILD_DIR)/dep.list
