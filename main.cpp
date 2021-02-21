#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <CL/opencl.hpp>

#define WIDTH (300 * 4)
#define HEIGHT (200 * 4)
#define N_ITERS 1000


using set_type = long double;


void init_pixels(sf::Uint8 *setArray) {
    for (int i = 0; i < WIDTH * HEIGHT * 4; i += 4) {
        setArray[i] = 0;
        setArray[i + 1] = 255;
        setArray[i + 3] = 255;
    }
}

int main() {
    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);
    if(all_platforms.empty()){
        std::cout<<" No platforms found. Check OpenCL installation!\n";
        exit(1);
    }
    const char *KernelSource =
            R"("
        #include "mandelbrot.cl"
        ")"
            ;

    const cl_uint num = 1;
    clGetDeviceIDs(nullptr, CL_DEVICE_TYPE_GPU, 0, nullptr, (cl_uint*)&num);

    cl_device_id devices[1];
    std::cout << devices[0] << std::endl;
    clGetDeviceIDs(nullptr, CL_DEVICE_TYPE_GPU, num, devices, nullptr);
    std::cout << devices[0] << std::endl;
    // create a compute context with GPU device
    cl_context context = clCreateContextFromType(nullptr, CL_DEVICE_TYPE_GPU, nullptr, nullptr, nullptr);

    // create a command queue
    clGetDeviceIDs(nullptr, CL_DEVICE_TYPE_DEFAULT, 1, devices, nullptr);
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, devices[0], nullptr, nullptr);

    cl_mem memobjs = clCreateBuffer(context,
                                        CL_MEM_READ_WRITE,
                                        sizeof(sf::Uint8) * 4 * WIDTH * HEIGHT,
                                        nullptr,
                                        nullptr);

	cl_program program = clCreateProgramWithSource(context, 1, (const char **)& KernelSource, nullptr, nullptr);

    clBuildProgram(program, 0, nullptr, nullptr, nullptr, nullptr);
    cl_kernel kernel = clCreateKernel(program, "mandelbrot", nullptr);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobjs);

    size_t global_work_size[1] = { WIDTH * HEIGHT * 4 };
    size_t local_work_size[1] = { 256 };

    auto *pixels = new sf::Uint8[WIDTH * HEIGHT * 4];
    init_pixels(pixels);

    clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, global_work_size, local_work_size, 0, nullptr, nullptr);
    clEnqueueReadBuffer(queue, memobjs, CL_TRUE, 0, WIDTH * HEIGHT * sizeof(sf::Uint8) * 4, pixels, 0, nullptr, nullptr);


    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot");
    sf::Texture texture;
    texture.create(WIDTH, HEIGHT);
    bool generate_new = true;
    sf::Sprite sprite(texture);
    unsigned short n_iters = N_ITERS;
    set_type divider = 1.;
    set_type real_shift = 0.7;
    set_type imag_shift = 0;

    while (window.isOpen())
    {
        sf::Event event{};
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::MouseWheelMoved:
                if (event.mouseWheel.delta == -1)
                    divider /= 1.02;
                else
                    divider *= 1.02;
                generate_new = true;
                break;
            case sf::Event::KeyPressed:
                switch (event.key.code) {
                case sf::Keyboard::Left:
                    real_shift += 0.1 / divider;
                    break;
                case sf::Keyboard::Right:
                    real_shift -= 0.1 / divider;
                    break;
                case sf::Keyboard::Up:
                    imag_shift += 0.1 / divider;
                    break;
                case sf::Keyboard::K:
                    divider *= 1.02;
                    break;
                case sf::Keyboard::J:
                    divider /= 1.02;
                    break;
                case sf::Keyboard::R:
                    divider = 1;
                    break;
                case sf::Keyboard::Down:
                    imag_shift -= 0.1 / divider;
                    break;
                case sf::Keyboard::H:
                    n_iters -= 10;
                    if (n_iters <= 0)
                        n_iters = 10;
                    break;
                case sf::Keyboard::L:
                    n_iters += 10;
                    break;
                default:
                    break;
                }
                generate_new = true;
                break;
            default:
                break;
            }
        }
        if (generate_new) {
            n_iters = std::max(30, int(std::log(divider) * 40));

//            generate_set(pixels, divider, real_shift, imag_shift, n_iters);
            generate_new = false;
            std::cout << divider << "  " << n_iters << "  " << std::log(divider) << std::endl;
        }
        texture.update(pixels);
        window.draw(sprite);
        window.display();
    }
    delete[] pixels;
    return 0;
}