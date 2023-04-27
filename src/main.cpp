#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

#include <ppmpp.hpp>
#include <tiny_obj_loader.h>
#include <Eigen/Dense>
#include "src/Core/Matrix.h"

#include "log.hpp"
#include "ray.hpp"

using namespace ppm;

Color ray_color(const Ray &r);

const int WIDTH = 1920;
const int HEIGHT = 1080;

int main() {
    // image
    float aspect = float(WIDTH) / float(HEIGHT);
    Image image(WIDTH, HEIGHT);
    
    // camera
    float viewport_height = 2.0;
    float viewport_width = aspect * viewport_height;
    float focal_length = 1.0;

    Vector3f origin = Vector3f(0, 0, 0);
    Vector3f horizontal = Vector3f(viewport_width, 0, 0);
    Vector3f vertical = Vector3f(0, viewport_height, 0);
    auto lower_left_corner = origin - 
        horizontal / 2 - vertical / 2 -
        Vector3f(0, 0, focal_length);

    // render
    for (int h = 0; h < HEIGHT; h++) {
        for (int w = 0; w < WIDTH; w++) {
            float u = float(w) / (WIDTH - 1);
            float v = float(h) / (HEIGHT - 1);
            Ray r(origin, lower_left_corner + u*horizontal + v*vertical - origin);
            Color color = ray_color(r);
            image.set(w, h, color);
            print_progress(h*WIDTH+w, WIDTH*HEIGHT, 40);
        }
    }
    std::cout << std::endl << "Done." << std::endl;

    image.save("../image.ppm");

    return 0;
}

Color ray_color(const Ray &r) {
    Vector3f unit_direction = r.direction().normalized();
    auto t = 0.5 * (unit_direction.y() + 1.0);

    auto c1 = Color(255, 255, 255);
    c1.intensity(1.0-t);
    auto c2 = Color(127, 178, 255);
    c2.intensity(t);

    return c1 + c2;
}
